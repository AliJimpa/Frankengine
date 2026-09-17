@echo off
setlocal enabledelayedexpansion

:: --- ANSI colors (works in VSCode terminal / Windows Terminal / modern cmd) ---
for /f %%a in ('echo prompt $E^| cmd') do set "ESC=%%a"
set "RED=%ESC%[91m"
set "GREEN=%ESC%[92m"
set "RESET=%ESC%[0m"

:: ============================================================
:: compile.bat
:: Compiles .cpp files under <ProjectRoot>\Assets into
:: <ProjectRoot>\Intermediate\Assets, mirroring the folder layout.
::
:: Config\build.ini's [Compile] section then adjusts specific
:: top-level Assets subfolders:
::   Name=skip    -> that whole subtree is never compiled at all
::   Name=include -> compiled normally as part of Assets, then
::                   MOVED out to its own \Intermediate\Name
::                   folder once compiling finishes
:: Anything not listed stays in \Intermediate\Assets.
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

:: --- Read Config\build.ini (or game.ini): [Build] CRT, and [Compile] folders ---
set "CRTFLAG=/MD"
set "COMPILECOUNT=0"
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
                    if /I "!SECTION!"=="[Build]" if /I "%%A"=="CRT" (
                        if /I "%%B"=="MT" set "CRTFLAG=/MT"
                        if /I "%%B"=="MD" set "CRTFLAG=/MD"
                    )
                    if /I "!SECTION!"=="[Compile]" (
                        set /a COMPILECOUNT+=1
                        set "COMPILE_NAME[!COMPILECOUNT!]=%%A"
                        set "COMPILE_STATUS[!COMPILECOUNT!]=%%B"
                    )
                )
            )
        )
    )
) else (
    echo [INFO] No Config\build.ini found, defaulting to /MD, no special folders.
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
echo ============================================================

set COUNT=0
set FAILED=0

echo --- Compiling Assets ^(skip-listed folders excluded^) ---
call :CompileTree "%ASSETS_DIR%" "%INTERMEDIATE_DIR%\Assets" 1

if !COMPILECOUNT! GTR 0 (
    echo --- Relocating 'include' folders out of Assets ---
    for /L %%I in (1,1,!COMPILECOUNT!) do (
        set "CNAME=!COMPILE_NAME[%%I]!"
        set "CSTATUS=!COMPILE_STATUS[%%I]!"
        set "SRCMOVE=%INTERMEDIATE_DIR%\Assets\!CNAME!"
        set "DSTMOVE=%INTERMEDIATE_DIR%\!CNAME!"
        if /I "!CSTATUS!"=="skip" (
            if exist "%INTERMEDIATE_DIR%\Assets\!CNAME!" (
                rmdir /s /q "%INTERMEDIATE_DIR%\Assets\!CNAME!"
                echo [SKIP] Assets\!CNAME! ^(status=skip - removed stale output^)
            ) else (
                echo [SKIP] Assets\!CNAME! ^(status=skip, not compiled^)
            )
        ) else if /I "!CSTATUS!"=="include" (
            if exist "!SRCMOVE!" (
                if exist "!DSTMOVE!" rmdir /s /q "!DSTMOVE!"
                move /Y "!SRCMOVE!" "!DSTMOVE!" >nul
                echo [MOVED] Assets\!CNAME!  -^>  Intermediate\!CNAME!
            ) else (
                echo [INFO] No compiled output for !CNAME! ^(Assets\!CNAME! not found or empty^)
            )
        ) else (
            echo !RED![WARN] Unknown status '!CSTATUS!' for !CNAME!, treating as skip.!RESET!
        )
    )
)

echo ============================================================
if %FAILED% GTR 0 (
    echo !GREEN!Compiled: %COUNT%!RESET!   !RED!Failed: %FAILED%!RESET!
) else (
    echo !GREEN!Compiled: %COUNT%   Failed: %FAILED%!RESET!
)
echo Object files are in: %INTERMEDIATE_DIR%\Assets ^(and any listed [Compile] modules^)
echo ============================================================

if %FAILED% GTR 0 exit /b 1
exit /b 0

:: ============================================================
:: :CompileTree  -  compile every .cpp under %1 into %2 (mirroring
:: subfolder layout). %3=1 excludes anything under a top-level
:: Assets subfolder listed in [Compile] (used for the default
:: "Assets" bucket pass only, so listed folders get compiled by
:: their own dedicated call instead, or not at all if skip).
:: ============================================================
:CompileTree
set "_SRC=%~1"
set "_OBJROOT=%~2"
set "_APPLYEXCLUDE=%~3"

if not exist "%_OBJROOT%" mkdir "%_OBJROOT%"

for /r "%_SRC%" %%F in (*.cpp) do (
    set "SRC=%%F"
    if "%_APPLYEXCLUDE%"=="1" (
        call :CheckExclude "!SRC!"
    ) else (
        set "SKIPFILE=0"
    )

    if "!SKIPFILE!"=="0" (
        set "RELDIR=%%~dpF"
        set "RELDIR=!RELDIR:%_SRC%\=!"
        if not exist "%_OBJROOT%\!RELDIR!" mkdir "%_OBJROOT%\!RELDIR!"

        set "OBJ=%_OBJROOT%\!RELDIR!%%~nF.obj"
        set "PDB=%_OBJROOT%\!RELDIR!%%~nF.pdb"

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
)
exit /b 0

:CheckExclude
set "_FILE=%~1"
set "SKIPFILE=0"
if !COMPILECOUNT! GTR 0 (
    for /L %%J in (1,1,!COMPILECOUNT!) do (
        set "EXNAME=!COMPILE_NAME[%%J]!"
        set "EXSTATUS=!COMPILE_STATUS[%%J]!"
        if /I "!EXSTATUS!"=="skip" (
            echo !_FILE!| findstr /I /B /C:"%ASSETS_DIR%\!EXNAME!" >nul
            if not errorlevel 1 set "SKIPFILE=1"
        )
    )
)
exit /b 0