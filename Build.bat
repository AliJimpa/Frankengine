@echo off
setlocal enabledelayedexpansion

:: --- ANSI colors (works in VSCode terminal / Windows Terminal / modern cmd) ---
for /f %%a in ('echo prompt $E^| cmd') do set "ESC=%%a"
set "RED=%ESC%[91m"
set "GREEN=%ESC%[92m"
set "RESET=%ESC%[0m"

:: ============================================================
:: build.bat
:: Links all .obj files from <ProjectRoot>\Intermediate (and any
:: .lib files from <ProjectRoot>\Library) into an .exe, then
:: copies every .dll from Library into the target build folder.
::
:: Usage:
::   build.bat "C:\Path\To\ProjectRoot" "C:\Path\To\BuildOutput" [ExeName.exe]
:: ============================================================

if "%~2"=="" (
    echo Usage: %~nx0 "ProjectRootPath" "OutputBuildFolder" [ExeName.exe]
    exit /b 1
)

set "PROJECT_DIR=%~1"
if "%PROJECT_DIR:~-1%"=="\" set "PROJECT_DIR=%PROJECT_DIR:~0,-1%"
set "OUTPUT_DIR=%~2"
set "EXE_NAME=%~3"
if "%EXE_NAME%"=="" (
    for %%P in ("%PROJECT_DIR%") do set "EXE_NAME=%%~nxP.exe"
)

set "INTERMEDIATE_DIR=%PROJECT_DIR%\Intermediate"
set "LIBRARY_DIR=%PROJECT_DIR%\Library"

if not exist "%INTERMEDIATE_DIR%" (
    echo [ERROR] Intermediate folder not found: %INTERMEDIATE_DIR%
    echo         Run compile.bat first.
    exit /b 1
)

if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

:: --- Make sure link.exe is available (MSVC toolchain) ---
where link.exe >nul 2>nul
if errorlevel 1 (
    echo [INFO] link.exe not on PATH, trying to load VS build environment...
    :: EDIT this path to match your Visual Studio install/version
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    if errorlevel 1 (
        echo [ERROR] Could not initialize MSVC environment.
        echo         Edit the vcvars64.bat path inside this script.
        exit /b 1
    )
)

:: --- Collect all .obj files ---
set "OBJLIST="
for /r "%INTERMEDIATE_DIR%" %%F in (*.obj) do (
    set "OBJLIST=!OBJLIST! "%%F""
)

if "%OBJLIST%"=="" (
    echo [ERROR] No .obj files found in %INTERMEDIATE_DIR%
    exit /b 1
)

:: --- Read Config\build.ini (or game.ini): per-library skip/dynamic/static ---
set "LIBCOUNT=0"
set "INIFILE=%PROJECT_DIR%\Config\build.ini"
if not exist "%INIFILE%" set "INIFILE=%PROJECT_DIR%\Config\game.ini"

if exist "%INIFILE%" (
    echo Reading library config: %INIFILE%
    for /f "usebackq tokens=1,2 delims==" %%A in ("%INIFILE%") do (
        set "LNAME=%%A"
        set "LSTATUS=%%B"
        if not "!LNAME!"=="" if not "!LNAME:~0,1!"==";" if not "!LNAME:~0,1!"=="#" if not "!LNAME:~0,1!"=="[" (
            set /a LIBCOUNT+=1
            set "LIB_NAME[!LIBCOUNT!]=!LNAME!"
            set "LIB_STATUS[!LIBCOUNT!]=!LSTATUS!"
        )
    )
) else (
    echo [INFO] No Config\build.ini found, using every library under Library\.
)

:: --- Collect .lib files/paths from Library ---
set "LIBPATH="
set "LIBFILES="
if !LIBCOUNT! GTR 0 (
    for /L %%I in (1,1,!LIBCOUNT!) do (
        set "LNAME=!LIB_NAME[%%I]!"
        set "LSTATUS=!LIB_STATUS[%%I]!"
        set "LDIR=%LIBRARY_DIR%\!LNAME!"
        if /I "!LSTATUS!"=="skip" (
            echo [SKIP] !LNAME! ^(status=skip^)
        ) else if not exist "!LDIR!" (
            echo !RED![WARN] Library folder not found: !LDIR!!RESET!
        ) else (
            for /r "!LDIR!" %%F in (*.lib) do (
                set "LIBDIR2=%%~dpF"
                if "!LIBDIR2:~-1!"=="\" set "LIBDIR2=!LIBDIR2:~0,-1!"
                echo !LIBPATH! | find /I "!LIBDIR2!" >nul
                if errorlevel 1 set "LIBPATH=!LIBPATH! /LIBPATH:"!LIBDIR2!""
                set "LIBFILES=!LIBFILES! "%%~nxF""
            )
        )
    )
) else if exist "%LIBRARY_DIR%" (
    for /r "%LIBRARY_DIR%" %%F in (*.lib) do (
        set "LIBDIR=%%~dpF"
        if "!LIBDIR:~-1!"=="\" set "LIBDIR=!LIBDIR:~0,-1!"
        echo !LIBPATH! | find /I "!LIBDIR!" >nul
        if errorlevel 1 set "LIBPATH=!LIBPATH! /LIBPATH:"!LIBDIR!""
        set "LIBFILES=!LIBFILES! "%%~nxF""
    )
)

echo ============================================================
echo Linking -^> %OUTPUT_DIR%\%EXE_NAME%
echo ------------------------------------------------------------
echo [DEBUG] OBJLIST  = %OBJLIST%
echo [DEBUG] LIBPATH  = %LIBPATH%
echo [DEBUG] LIBFILES = %LIBFILES%
echo ============================================================

for %%N in ("%EXE_NAME%") do set "EXE_BASENAME=%%~nN"
:: --- Windows system libs GLFW's Win32/WGL backend needs ---
set "SYSLIBS=user32.lib gdi32.lib shell32.lib advapi32.lib opengl32.lib"

set "LINKLOG=%TEMP%\link_%RANDOM%.log"
link.exe /nologo /DEBUG /PDB:"%INTERMEDIATE_DIR%\%EXE_BASENAME%.pdb" /OUT:"%OUTPUT_DIR%\%EXE_NAME%" %LIBPATH% !OBJLIST! !LIBFILES! %SYSLIBS% > "!LINKLOG!" 2>&1
set "LINKERR=!ERRORLEVEL!"

for /f "usebackq delims=" %%L in ("!LINKLOG!") do (
    set "LINE=%%L"
    echo(!LINE!| findstr /I /C:"error" >nul
    if not errorlevel 1 (
        echo !RED!!LINE!!RESET!
    ) else (
        echo(!LINE!
    )
)
del "!LINKLOG!" >nul 2>&1

if !LINKERR! NEQ 0 (
    echo !RED![ERROR] Link step failed.!RESET!
    exit /b 1
)

echo ============================================================
echo Copying DLLs from dynamic libraries to build folder...
echo ============================================================
if !LIBCOUNT! GTR 0 (
    for /L %%I in (1,1,!LIBCOUNT!) do (
        set "LNAME=!LIB_NAME[%%I]!"
        set "LSTATUS=!LIB_STATUS[%%I]!"
        set "LDIR=%LIBRARY_DIR%\!LNAME!"
        if /I "!LSTATUS!"=="dynamic" if exist "!LDIR!" (
            for /r "!LDIR!" %%F in (*.dll) do (
                copy /Y "%%F" "%OUTPUT_DIR%\" >nul
                echo Copied: %%~nxF ^(!LNAME!^)
            )
        )
    )
) else if exist "%LIBRARY_DIR%" (
    for /r "%LIBRARY_DIR%" %%F in (*.dll) do (
        copy /Y "%%F" "%OUTPUT_DIR%\" >nul
        echo Copied: %%~nxF
    )
) else (
    echo [INFO] No Library folder found, skipping DLL copy.
)

echo ============================================================
echo !GREEN!Build complete: %OUTPUT_DIR%\%EXE_NAME%!RESET!
echo ============================================================
exit /b 0