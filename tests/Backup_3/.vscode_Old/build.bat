@echo off
setlocal enabledelayedexpansion

set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Development

set ROOT=%~dp0
set SRC_DIR=%ROOT%Assets
set LIB_DIR=%ROOT%Library
set INT_DIR=%ROOT%Intermediate\%CONFIG%
set OUT_DIR=%ROOT%Packaging\%CONFIG%
set PROJECT_NAME=Ali

if not exist "%INT_DIR%" mkdir "%INT_DIR%"
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"

if /I "%CONFIG%"=="Shipping" (
    set FLAGS=-O2 -DNDEBUG -std=c++17
) else (
    set FLAGS=-g -O0 -std=c++17
)

set OBJS=
for /r "%SRC_DIR%" %%f in (*.cpp) do (
    echo Compiling: %%f
    g++ %FLAGS% -I"%SRC_DIR%" -I"%LIB_DIR%" -c "%%f" -o "%INT_DIR%\%%~nf.o"
    set OBJS=!OBJS! "%INT_DIR%\%%~nf.o"
)

echo Linking: %PROJECT_NAME% (%CONFIG%)
g++ %FLAGS% !OBJS! -L"%LIB_DIR%" -o "%OUT_DIR%\%PROJECT_NAME%.exe"
echo Build complete: %OUT_DIR%\%PROJECT_NAME%.exe
