@echo off
setlocal EnableDelayedExpansion

REM ============================================================
REM BuildImGuiDLL.bat
REM
REM Usage:
REM     BuildImGuiDLL.bat "C:\Path\To\Project"
REM
REM
REM Input:
REM     Dependency\imgui
REM
REM Output:
REM     Intermediate\imgui_dll\*.obj
REM     Library\imgui_opengl_dllx86\ImGui.dll
REM     Library\imgui_opengl_dllx86\*.h
REM ============================================================


REM ============================================================
REM Project Path
REM ============================================================

if "%~1"=="" (
    echo.
    echo ERROR: Project path was not provided.
    echo.
    echo Usage:
    echo     %~nx0 "C:\Path\To\Project"
    echo.
    exit /b 1
)

set "PROJECT_DIR=%~1"


REM ============================================================
REM Paths
REM ============================================================

set "IMGUI_DIR=%PROJECT_DIR%\Dependency\imgui"

set "GLFW_DIR=%PROJECT_DIR%\Dependency\glfw"

set "INTERMEDIATE_DIR=%PROJECT_DIR%\Intermediate\imgui_dll"

set "LIBRARY_DIR=%PROJECT_DIR%\Library\imgui_opengl_dllx86"

set "OUTPUT_DLL=%LIBRARY_DIR%\ImGui.dll"


REM ============================================================
REM HEADER FILE LIST
REM
REM These files are ONLY copied.
REM They are NOT compiled.
REM ============================================================

set "HEADER_1=imgui.h"
set "HEADER_2=imgui_internal.h"
set "HEADER_3=imconfig.h"
set "HEADER_4=backends\imgui_impl_glfw.h"
set "HEADER_5=backends\imgui_impl_opengl2.h"


REM ============================================================
REM CPP FILE LIST
REM
REM These files are compiled into .obj files.
REM ============================================================

set "CPP_1=imgui.cpp"
set "CPP_2=imgui_draw.cpp"
set "CPP_3=imgui_tables.cpp"
set "CPP_4=imgui_widgets.cpp"

set "CPP_5=backends\imgui_impl_glfw.cpp"
set "CPP_6=backends\imgui_impl_opengl2.cpp"


REM ============================================================
REM Clean Target Library
REM ============================================================

echo.
echo ============================================================
echo Cleaning Library
echo ============================================================
echo.

if exist "%LIBRARY_DIR%" (
    echo Removing old files...
    del /q /f "%LIBRARY_DIR%\*" >nul 2>&1
)

if not exist "%LIBRARY_DIR%" (
    mkdir "%LIBRARY_DIR%"
)


REM ============================================================
REM Prepare Intermediate
REM ============================================================

if not exist "%INTERMEDIATE_DIR%" (
    mkdir "%INTERMEDIATE_DIR%"
)


REM ============================================================
REM Check Dependencies
REM ============================================================

if not exist "%IMGUI_DIR%" (
    echo.
    echo ERROR: ImGui dependency not found:
    echo %IMGUI_DIR%
    echo.
    exit /b 1
)

if not exist "%GLFW_DIR%\include" (
    echo.
    echo ERROR: GLFW include directory not found:
    echo %GLFW_DIR%\include
    echo.
    exit /b 1
)


REM ============================================================
REM Copy Header Files
REM ============================================================

echo.
echo ============================================================
echo Copying Header Files
echo ============================================================
echo.

for /L %%N in (1,1,5) do (

    for %%F in ("!HEADER_%%N!") do (

        set "SOURCE_HEADER=%IMGUI_DIR%\%%~F"
        set "HEADER_DIR=%%~dpF"

        if not exist "!SOURCE_HEADER!" (
            echo.
            echo ERROR: Header file not found:
            echo !SOURCE_HEADER!
            echo.
            exit /b 1
        )

        REM Create subdirectory if required

        if not "%%~dpF"=="" (
            if not exist "%LIBRARY_DIR%\%%~dpF" (
                mkdir "%LIBRARY_DIR%\%%~dpF"
            )
        )

        echo [COPY] %%F

        copy /Y "!SOURCE_HEADER!" "%LIBRARY_DIR%\%%~dpF" >nul

        if errorlevel 1 (
            echo.
            echo ERROR: Failed to copy:
            echo !SOURCE_HEADER!
            echo.
            exit /b 1
        )
    )
)


REM ============================================================
REM Compile CPP Files
REM ============================================================

echo.
echo ============================================================
echo Compiling CPP Files
echo ============================================================
echo.

for /L %%N in (1,1,6) do (

    for %%F in ("!CPP_%%N!") do (

        set "SOURCE_CPP=%IMGUI_DIR%\%%~F"
        set "CPP_NAME=%%~nF"

        if not exist "!SOURCE_CPP!" (
            echo.
            echo ERROR: CPP file not found:
            echo !SOURCE_CPP!
            echo.
            exit /b 1
        )

        echo [C++] %%F

        REM ----------------------------------------------------
        REM GLFW Backend
        REM ----------------------------------------------------

        if "%%~nxF"=="imgui_impl_glfw.cpp" (

            cl ^
                /nologo ^
                /c ^
                /EHsc ^
                /MD ^
                /I "%IMGUI_DIR%" ^
                /I "%GLFW_DIR%\include" ^
                "!SOURCE_CPP!" ^
                /Fo"%INTERMEDIATE_DIR%\!CPP_NAME!.obj"

        ) else (

            cl ^
                /nologo ^
                /c ^
                /EHsc ^
                /MD ^
                /I "%IMGUI_DIR%" ^
                "!SOURCE_CPP!" ^
                /Fo"%INTERMEDIATE_DIR%\!CPP_NAME!.obj"
        )

        if errorlevel 1 (
            echo.
            echo ERROR: Compilation failed:
            echo !SOURCE_CPP!
            echo.
            exit /b 1
        )
    )
)


REM ============================================================
REM Create DLL
REM ============================================================

echo.
echo ============================================================
echo Creating ImGui.dll
echo ============================================================
echo.

link ^
    /DLL ^
    /NOLOGO ^
    /OUT:"%OUTPUT_DLL%" ^
    "%INTERMEDIATE_DIR%\imgui.obj" ^
    "%INTERMEDIATE_DIR%\imgui_draw.obj" ^
    "%INTERMEDIATE_DIR%\imgui_tables.obj" ^
    "%INTERMEDIATE_DIR%\imgui_widgets.obj" ^
    "%INTERMEDIATE_DIR%\imgui_impl_glfw.obj" ^
    "%INTERMEDIATE_DIR%\imgui_impl_opengl2.obj" ^
    opengl32.lib


if errorlevel 1 (
    echo.
    echo ERROR: Failed to create ImGui.dll
    echo.
    exit /b 1
)


REM ============================================================
REM Done
REM ============================================================

echo.
echo ============================================================
echo ImGui DLL Build Complete
echo ============================================================
echo.

echo DLL:
echo %OUTPUT_DLL%

echo.
echo Headers:
echo %LIBRARY_DIR%

echo.
echo Intermediate:
echo %INTERMEDIATE_DIR%

echo.
echo ============================================================

exit /b 0