@echo off
setlocal enabledelayedexpansion

:: --- ANSI colors (works in VSCode terminal / Windows Terminal / modern cmd) ---
for /f %%a in ('echo prompt $E^| cmd') do set "ESC=%%a"
set "RED=%ESC%[91m"
set "GREEN=%ESC%[92m"
set "RESET=%ESC%[0m"

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

:: Module name = subfolder under \Intermediate that receives the .obj files.
:: Override with a 2nd argument if you ever compile another source tree.
set "MODULE_NAME=%~2"
if "%MODULE_NAME%"=="" set "MODULE_NAME=Project"
set "OBJ_ROOT=%INTERMEDIATE_DIR%\%MODULE_NAME%"

if not exist "%ASSETS_DIR%" (
    echo [ERROR] Assets folder not found: %ASSETS_DIR%
    exit /b 1
)

if not exist "%INTERMEDIATE_DIR%" mkdir "%INTERMEDIATE_DIR%"
if not exist "%OBJ_ROOT%" mkdir "%OBJ_ROOT%"

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

:: --- Read Config\build.ini (or game.ini) to decide /MD vs /MT ---
set "CRTFLAG=/MD"
set "STATIC_COUNT=0"
set "DYNAMIC_COUNT=0"
set "INIFILE=%PROJECT_DIR%\Config\build.ini"
if not exist "%INIFILE%" set "INIFILE=%PROJECT_DIR%\Config\game.ini"

if exist "%INIFILE%" (
    echo Reading library config: %INIFILE%
    set "SECTION="
    for /f "usebackq tokens=* delims=" %%R in ("%INIFILE%") do (
        set "RAW=%%R"
        if not "!RAW!"=="" if not "!RAW:~0,1!"==";" if not "!RAW:~0,1!"=="#" (
            if "!RAW:~0,1!"=="[" (
                set "SECTION=!RAW!"
            ) else if /I "!SECTION!"=="[Libraries]" (
                for /f "tokens=1,2 delims==" %%A in ("!RAW!") do (
                    if /I "%%B"=="static"  set /a STATIC_COUNT+=1
                    if /I "%%B"=="dynamic" set /a DYNAMIC_COUNT+=1
                )
            )
        )
    )
    if !STATIC_COUNT! GTR 0 set "CRTFLAG=/MT"
    if !STATIC_COUNT! GTR 0 if !DYNAMIC_COUNT! GTR 0 (
        echo !RED![WARN] build.ini mixes 'static' and 'dynamic' libraries.!RESET!
        echo !RED![WARN] Compiling with /MT - dynamic libraries expecting /MD may fail to link.!RESET!
    )
) else (
    echo [INFO] No Config\build.ini found, defaulting to /MD.
)
echo Using CRT mode: %CRTFLAG%

:: --- Include dirs: project root ^(for "Library/..." style includes^), Assets, Library ---
set "INCLUDES=/I"%PROJECT_DIR%""
set "INCLUDES=%INCLUDES% /I"%ASSETS_DIR%""
if exist "%LIBRARY_DIR%" set "INCLUDES=%INCLUDES% /I"%LIBRARY_DIR%""

echo ============================================================
echo Project     : %PROJECT_DIR%
echo Assets      : %ASSETS_DIR%
echo Intermediate: %INTERMEDIATE_DIR%
echo Module      : %MODULE_NAME%  -^>  %OBJ_ROOT%
echo ============================================================

set COUNT=0
set FAILED=0

for /r "%ASSETS_DIR%" %%F in (*.cpp) do (
    set "SRC=%%F"
    set "RELDIR=%%~dpF"
    set "RELDIR=!RELDIR:%ASSETS_DIR%\=!"

    if not exist "%OBJ_ROOT%\!RELDIR!" mkdir "%OBJ_ROOT%\!RELDIR!"

    set "OBJ=%OBJ_ROOT%\!RELDIR!%%~nF.obj"
    set "PDB=%OBJ_ROOT%\!RELDIR!%%~nF.pdb"

    echo Compiling: %%~nxF
    set "CLLOG=%TEMP%\cl_%RANDOM%.log"
    cl.exe /c /nologo /EHsc /Zi /FS /Od %CRTFLAG% %INCLUDES% "!SRC!" /Fo"!OBJ!" /Fd"!PDB!" > "!CLLOG!" 2>&1
    set "CLERR=!ERRORLEVEL!"

    for /f "usebackq delims=" %%L in ("!CLLOG!") do (
        set "LINE=%%L"
        echo(!LINE!| findstr /I /C:"error" >nul
        if not errorlevel 1 (
            echo !RED!!LINE!!RESET!
        ) else (
            echo(!LINE!
        )
    )
    del "!CLLOG!" >nul 2>&1

    if !CLERR! NEQ 0 (
        echo !RED![FAILED] %%~nxF!RESET!
        set /a FAILED+=1
    ) else (
        set /a COUNT+=1
    )
)

echo ============================================================
if %FAILED% GTR 0 (
    echo !GREEN!Compiled: %COUNT%!RESET!   !RED!Failed: %FAILED%!RESET!
) else (
    echo !GREEN!Compiled: %COUNT%   Failed: %FAILED%!RESET!
)
echo Object files are in: %OBJ_ROOT%
echo ============================================================

if %FAILED% GTR 0 exit /b 1
exit /b 0