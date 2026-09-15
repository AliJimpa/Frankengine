@echo off
setlocal

:: ============================================================
:: clean.bat
:: Deletes and recreates <ProjectRoot>\Intermediate (including all
:: module subfolders such as \Intermediate\Project) and
:: <ProjectRoot>\Binaries\Win64 so the next build starts fresh.
::
:: Usage:
::   clean.bat "C:\Path\To\ProjectRoot"
:: ============================================================

if "%~1"=="" (
    echo Usage: %~nx0 "ProjectRootPath"
    exit /b 1
)

set "PROJECT_DIR=%~1"
set "INTERMEDIATE_DIR=%PROJECT_DIR%\Intermediate"
set "BUILD_DIR=%PROJECT_DIR%\Binaries\Win64"

echo ============================================================
echo Cleaning: %INTERMEDIATE_DIR%
echo ============================================================
if exist "%INTERMEDIATE_DIR%" rmdir /s /q "%INTERMEDIATE_DIR%"
mkdir "%INTERMEDIATE_DIR%"

echo ============================================================
echo Cleaning: %BUILD_DIR%
echo ============================================================
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"

echo ============================================================
echo Clean complete.
echo ============================================================
exit /b 0