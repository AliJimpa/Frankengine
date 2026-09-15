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
::   build.bat "C:\Path\To\ProjectRoot" "C:\Path\To\ProjectRoot\Binaries\Win64" [ExeName.exe]
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

:: --- Read Config\build.ini (or game.ini): [Modules] and [Libraries] ---
set "LIBCOUNT=0"
set "MODCOUNT=0"
set "INIFILE=%PROJECT_DIR%\Config\build.ini"
if not exist "%INIFILE%" set "INIFILE=%PROJECT_DIR%\Config\game.ini"

if exist "%INIFILE%" (
    echo Reading build config: %INIFILE%
    set "SECTION="
    for /f "usebackq tokens=* delims=" %%R in ("%INIFILE%") do (
        set "RAW=%%R"
        if not "!RAW!"=="" if not "!RAW:~0,1!"==";" if not "!RAW:~0,1!"=="#" (
            if "!RAW:~0,1!"=="[" (
                set "SECTION=!RAW!"
            ) else (
                for /f "tokens=1,2 delims==" %%A in ("!RAW!") do (
                    if /I "!SECTION!"=="[Libraries]" (
                        set /a LIBCOUNT+=1
                        set "LIB_NAME[!LIBCOUNT!]=%%A"
                        set "LIB_STATUS[!LIBCOUNT!]=%%B"
                    )
                    if /I "!SECTION!"=="[Modules]" (
                        set /a MODCOUNT+=1
                        set "MOD_NAME[!MODCOUNT!]=%%A"
                        set "MOD_STATUS[!MODCOUNT!]=%%B"
                    )
                )
            )
        )
    )
) else (
    echo [INFO] No Config\build.ini found, using all objs and libraries found on disk.
)

:: --- Collect .obj files from the module folders listed in [Modules] ---
set "OBJLIST="
if !MODCOUNT! GTR 0 (
    for /L %%I in (1,1,!MODCOUNT!) do (
        set "MNAME=!MOD_NAME[%%I]!"
        set "MSTATUS=!MOD_STATUS[%%I]!"
        set "MDIR=%INTERMEDIATE_DIR%\!MNAME!"
        if /I "!MSTATUS!"=="skip" (
            echo [SKIP] module !MNAME! ^(status=skip^)
        ) else if not exist "!MDIR!" (
            echo !RED![WARN] Module folder not found: !MDIR!!RESET!
        ) else (
            echo [MODULE] !MNAME!
            call :CollectObjs "!MDIR!"
        )
    )
) else (
    call :CollectObjs "%INTERMEDIATE_DIR%"
)

if "!OBJLIST!"=="" (
    echo !RED![ERROR] No .obj files found. Run compile.bat first.!RESET!
    exit /b 1
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
            call :CollectLibs "!LDIR!"
        )
    )
) else if exist "%LIBRARY_DIR%" (
    call :CollectLibs "%LIBRARY_DIR%"
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
echo Copying DLLs from included libraries to build folder...
echo ============================================================
if !LIBCOUNT! GTR 0 (
    for /L %%I in (1,1,!LIBCOUNT!) do (
        set "LNAME=!LIB_NAME[%%I]!"
        set "LSTATUS=!LIB_STATUS[%%I]!"
        set "LDIR=%LIBRARY_DIR%\!LNAME!"
        if /I not "!LSTATUS!"=="skip" if exist "!LDIR!" (
            call :CopyDlls "!LDIR!" "!LNAME!"
        )
    )
) else if exist "%LIBRARY_DIR%" (
    call :CopyDlls "%LIBRARY_DIR%" "Library"
) else (
    echo [INFO] No Library folder found, skipping DLL copy.
)

echo ============================================================
echo !GREEN!Build complete: %OUTPUT_DIR%\%EXE_NAME%!RESET!
echo ============================================================
exit /b 0

:: ============================================================
:: Subroutines (called, not fallen into - script always exits above)
:: ============================================================

:CollectObjs
set "_DIR=%~1"
set "_N=0"
for /r "%_DIR%" %%F in (*.obj) do (
    set "OBJLIST=!OBJLIST! "%%F""
    set /a _N+=1
)
echo   -^> found !_N! .obj file^(s^) in %_DIR%
exit /b 0

:CollectLibs
set "_DIR=%~1"
set "_N=0"
for /r "%_DIR%" %%F in (*.lib) do (
    set "_LIBDIR=%%~dpF"
    if "!_LIBDIR:~-1!"=="\" set "_LIBDIR=!_LIBDIR:~0,-1!"
    echo !LIBPATH! | find /I "!_LIBDIR!" >nul
    if errorlevel 1 set "LIBPATH=!LIBPATH! /LIBPATH:"!_LIBDIR!""
    set "LIBFILES=!LIBFILES! "%%~nxF""
    set /a _N+=1
)
echo   -^> found !_N! .lib file^(s^) in %_DIR%
exit /b 0

:CopyDlls
set "_DIR=%~1"
set "_TAG=%~2"
for /r "%_DIR%" %%F in (*.dll) do (
    copy /Y "%%F" "%OUTPUT_DIR%\" >nul
    echo Copied: %%~nxF ^(%_TAG%^)
)
exit /b 0