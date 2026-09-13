@echo off
setlocal enabledelayedexpansion

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
if "%EXE_NAME%"=="" set "EXE_NAME=Game.exe"

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

:: --- Collect .lib files/paths from Library (including subfolders) ---
set "LIBPATH="
set "LIBFILES="
if exist "%LIBRARY_DIR%" (
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

link.exe /nologo /DEBUG /PDB:"%INTERMEDIATE_DIR%\%EXE_BASENAME%.pdb" /OUT:"%OUTPUT_DIR%\%EXE_NAME%" %LIBPATH% !OBJLIST! !LIBFILES! %SYSLIBS%

if errorlevel 1 (
    echo [ERROR] Link step failed.
    exit /b 1
)

echo ============================================================
echo Copying DLLs from Library to build folder...
echo ============================================================
if exist "%LIBRARY_DIR%" (
    for /r "%LIBRARY_DIR%" %%F in (*.dll) do (
        copy /Y "%%F" "%OUTPUT_DIR%\" >nul
        echo Copied: %%~nxF
    )
) else (
    echo [INFO] No Library folder found, skipping DLL copy.
)

echo ============================================================
echo Build complete: %OUTPUT_DIR%\%EXE_NAME%
echo ============================================================
exit /b 0