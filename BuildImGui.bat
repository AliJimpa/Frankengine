```bat
@echo off
setlocal

REM ============================================================
REM BuildImGui.bat
REM
REM Usage:
REM     BuildImGui.bat "C:\Path\To\Project"
REM
REM Output:
REM     Intermediate\*.obj
REM     Library\imgui_opengl_libx86\ImGui.lib
REM ============================================================


REM ============================================================
REM Project Paths
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

set "IMGUI_DIR=%PROJECT_DIR%\Dependency\imgui"
set "GLFW_DIR=%PROJECT_DIR%\Dependency\glfw-3.5.1.bin.WIN32"

set "INTERMEDIATE_DIR=%PROJECT_DIR%\Intermediate\imgui"
set "LIBRARY_DIR=%PROJECT_DIR%\Library\imgui_opengl_libx86"

set "OUTPUT_LIB=%LIBRARY_DIR%\ImGui.lib"


REM ============================================================
REM Create Directories
REM ============================================================

echo.
echo ============================================================
echo Preparing Directories
echo ============================================================
echo.

if not exist "%INTERMEDIATE_DIR%" (
    mkdir "%INTERMEDIATE_DIR%"
)

if not exist "%LIBRARY_DIR%" (
    mkdir "%LIBRARY_DIR%"
)


REM ============================================================
REM Check Dependencies
REM ============================================================

if not exist "%IMGUI_DIR%\imgui.cpp" (
    echo.
    echo ERROR: ImGui not found:
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
REM Compile ImGui Core
REM ============================================================

echo.
echo ============================================================
echo ImGui: Compile Core
echo ============================================================
echo.


echo [C++] imgui.cpp

cl ^
    /nologo ^
    /c ^
    /EHsc ^
    /MD ^
    /I "%IMGUI_DIR%" ^
    "%IMGUI_DIR%\imgui.cpp" ^
    /Fo"%INTERMEDIATE_DIR%\imgui.obj"

if errorlevel 1 exit /b 1


echo [C++] imgui_draw.cpp

cl ^
    /nologo ^
    /c ^
    /EHsc ^
    /MD ^
    /I "%IMGUI_DIR%" ^
    "%IMGUI_DIR%\imgui_draw.cpp" ^
    /Fo"%INTERMEDIATE_DIR%\imgui_draw.obj"

if errorlevel 1 exit /b 1


echo [C++] imgui_tables.cpp

cl ^
    /nologo ^
    /c ^
    /EHsc ^
    /MD ^
    /I "%IMGUI_DIR%" ^
    "%IMGUI_DIR%\imgui_tables.cpp" ^
    /Fo"%INTERMEDIATE_DIR%\imgui_tables.obj"

if errorlevel 1 exit /b 1


echo [C++] imgui_widgets.cpp

cl ^
    /nologo ^
    /c ^
    /EHsc ^
    /MD ^
    /I "%IMGUI_DIR%" ^
    "%IMGUI_DIR%\imgui_widgets.cpp" ^
    /Fo"%INTERMEDIATE_DIR%\imgui_widgets.obj"

if errorlevel 1 exit /b 1


REM ============================================================
REM Compile GLFW Backend
REM ============================================================

echo.
echo ============================================================
echo ImGui: Compile GLFW Backend
echo ============================================================
echo.

echo [C++] imgui_impl_glfw.cpp

cl ^
    /nologo ^
    /c ^
    /EHsc ^
    /MD ^
    /I "%GLFW_DIR%\include" ^
    /I "%IMGUI_DIR%" ^
    "%IMGUI_DIR%\backends\imgui_impl_glfw.cpp" ^
    /Fo"%INTERMEDIATE_DIR%\imgui_impl_glfw.obj"

if errorlevel 1 exit /b 1


REM ============================================================
REM Compile OpenGL2 Backend
REM ============================================================

echo.
echo ============================================================
echo ImGui: Compile OpenGL2 Backend
echo ============================================================
echo.

echo [C++] imgui_impl_opengl2.cpp

cl ^
    /nologo ^
    /c ^
    /EHsc ^
    /MD ^
    /I "%IMGUI_DIR%" ^
    "%IMGUI_DIR%\backends\imgui_impl_opengl2.cpp" ^
    /Fo"%INTERMEDIATE_DIR%\imgui_impl_opengl2.obj"

if errorlevel 1 exit /b 1


REM ============================================================
REM Create ImGui Library
REM ============================================================

echo.
echo ============================================================
echo ImGui: Create Library
echo ============================================================
echo.

lib ^
    /nologo ^
    /OUT:"%OUTPUT_LIB%" ^
    "%INTERMEDIATE_DIR%\imgui.obj" ^
    "%INTERMEDIATE_DIR%\imgui_draw.obj" ^
    "%INTERMEDIATE_DIR%\imgui_tables.obj" ^
    "%INTERMEDIATE_DIR%\imgui_widgets.obj" ^
    "%INTERMEDIATE_DIR%\imgui_impl_glfw.obj" ^
    "%INTERMEDIATE_DIR%\imgui_impl_opengl2.obj"

if errorlevel 1 (
    echo.
    echo ERROR: Failed to create ImGui.lib
    echo.
    exit /b 1
)


REM ============================================================
REM Done
REM ============================================================

echo.
echo ============================================================
echo ImGui Build Complete
echo ============================================================
echo.
echo Library:
echo %OUTPUT_LIB%
echo.
echo ============================================================

exit /b 0
```

### Why this version works

Instead of:

```bat
cl imgui.cpp imgui_draw.cpp ... /Fo"Intermediate\"
```

we now do:

```bat
cl imgui.cpp /Fo"Intermediate\imgui.obj"

cl imgui_draw.cpp /Fo"Intermediate\imgui_draw.obj"

cl imgui_tables.cpp /Fo"Intermediate\imgui_tables.obj"

cl imgui_widgets.cpp /Fo"Intermediate\imgui_widgets.obj"
```

This is exactly what MSVC expects.

Your final output should be:

```text
Intermediate
└── imgui
    ├── imgui.obj
    ├── imgui_draw.obj
    ├── imgui_tables.obj
    ├── imgui_widgets.obj
    ├── imgui_impl_glfw.obj
    └── imgui_impl_opengl2.obj

Library
└── imgui_opengl_libx86
    └── ImGui.lib
```

And then your original linker problem with:

```text
ImGui::StyleColorsDark
ImGui::Separator
```

should be resolved because `imgui_draw.obj`, `imgui_tables.obj`, and `imgui_widgets.obj` are now actually inside `ImGui.lib`.
