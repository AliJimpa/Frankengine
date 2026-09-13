@echo off
setlocal enabledelayedexpansion

:: ============================================================
:: compile.bat
:: Compiles every .cpp under <ProjectRoot>\Assets into .obj files
:: under <ProjectRoot>\Intermediate (mirroring subfolder layout).
::
:: Usage:
::   compile.bat "C:\Path\To\ProjectRoot"
:: ============================================================

if "%~1"=="" (
    echo Usage: %~nx0 "ProjectRootPath"
    exit /b 1
)

set "PROJECT_DIR=%~1"
if "%PROJECT_DIR:~-1%"=="\" set "PROJECT_DIR=%PROJECT_DIR:~0,-1%"
set "ASSETS_DIR=%PROJECT_DIR%\Assets"
set "INTERMEDIATE_DIR=%PROJECT_DIR%\Intermediate"
set "LIBRARY_DIR=%PROJECT_DIR%\Library"

if not exist "%ASSETS_DIR%" (
    echo [ERROR] Assets folder not found: %ASSETS_DIR%
    exit /b 1
)

if not exist "%INTERMEDIATE_DIR%" mkdir "%INTERMEDIATE_DIR%"

:: --- Make sure cl.exe is available (MSVC toolchain) ---
where cl.exe >nul 2>nul
if errorlevel 1 (
    echo [INFO] cl.exe not on PATH, trying to load VS build environment...
    :: EDIT this path to match your Visual Studio install/version
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    if errorlevel 1 (
        echo [ERROR] Could not initialize MSVC environment.
        echo         Edit the vcvars64.bat path inside this script.
        exit /b 1
    )
)

:: --- Include dirs: project root ^(for "Library/..." style includes^), Assets, Library ---
set "INCLUDES=/I"%PROJECT_DIR%""
set "INCLUDES=%INCLUDES% /I"%ASSETS_DIR%""
if exist "%LIBRARY_DIR%" set "INCLUDES=%INCLUDES% /I"%LIBRARY_DIR%""

echo ============================================================
echo Project     : %PROJECT_DIR%
echo Assets      : %ASSETS_DIR%
echo Intermediate: %INTERMEDIATE_DIR%
echo ============================================================

set COUNT=0
set FAILED=0

for /r "%ASSETS_DIR%" %%F in (*.cpp) do (
    set "SRC=%%F"
    set "RELDIR=%%~dpF"
    set "RELDIR=!RELDIR:%ASSETS_DIR%\=!"

    if not exist "%INTERMEDIATE_DIR%\!RELDIR!" mkdir "%INTERMEDIATE_DIR%\!RELDIR!"

    set "OBJ=%INTERMEDIATE_DIR%\!RELDIR!%%~nF.obj"
    set "PDB=%INTERMEDIATE_DIR%\!RELDIR!%%~nF.pdb"

    echo Compiling: %%~nxF
    cl.exe /c /nologo /EHsc /Zi /FS /Od /MD %INCLUDES% "!SRC!" /Fo"!OBJ!" /Fd"!PDB!"

    if errorlevel 1 (
        echo    [FAILED] %%~nxF
        set /a FAILED+=1
    ) else (
        set /a COUNT+=1
    )
)

echo ============================================================
echo Done. Compiled: %COUNT%   Failed: %FAILED%
echo Object files are in: %INTERMEDIATE_DIR%
echo ============================================================

if %FAILED% GTR 0 exit /b 1
exit /b 0