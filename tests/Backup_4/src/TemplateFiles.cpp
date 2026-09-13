#include "TemplateFiles.h"

namespace Templates
{

// tasks.json:
//  - "Build Development" -> Debug-style build, output to Packaging/Development
//  - "Build Shipping"    -> Release-style build, output to Packaging/Shipping
//  - "Clean Intermediate" -> wipes Intermediate/
// Each build task calls build.sh (Linux/macOS) or build.bat (Windows),
// which does the real compiling + linking (see BuildScriptSh/Bat below).
std::string TasksJson()
{
    return R"JSON({
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build Development",
            "type": "shell",
            "command": "${workspaceFolder}/build.sh",
            "args": ["Development"],
            "windows": {
                "command": "${workspaceFolder}\\build.bat",
                "args": ["Development"]
            },
            "group": { "kind": "build", "isDefault": true },
            "problemMatcher": ["$gcc"],
            "presentation": { "reveal": "always", "panel": "shared" }
        },
        {
            "label": "Build Shipping",
            "type": "shell",
            "command": "${workspaceFolder}/build.sh",
            "args": ["Shipping"],
            "windows": {
                "command": "${workspaceFolder}\\build.bat",
                "args": ["Shipping"]
            },
            "group": "build",
            "problemMatcher": ["$gcc"],
            "presentation": { "reveal": "always", "panel": "shared" }
        },
        {
            "label": "Clean Intermediate",
            "type": "shell",
            "command": "rm",
            "args": ["-rf", "${workspaceFolder}/Intermediate/Development", "${workspaceFolder}/Intermediate/Shipping"],
            "windows": {
                "command": "cmd",
                "args": ["/c", "if exist \"${workspaceFolder}\\Intermediate\" (rmdir /s /q \"${workspaceFolder}\\Intermediate\" & mkdir \"${workspaceFolder}\\Intermediate\")"]
            },
            "problemMatcher": []
        }
    ]
})JSON";
}

std::string LaunchJson(const std::string& projectName)
{
    std::string result = R"JSON({
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug (Development)",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/Packaging/Development/__NAME__",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "preLaunchTask": "Build Development"
        }
    ]
})JSON";
    size_t pos = result.find("__NAME__");
    result.replace(pos, 8, projectName);
    return result;
}

// IntelliSense picks up headers from Assets (your project sources) and
// Library (third-party / API headers).
std::string CppPropertiesJson()
{
    return R"JSON({
    "configurations": [
        {
            "name": "Generic",
            "includePath": [
                "${workspaceFolder}/Assets/**",
                "${workspaceFolder}/Library/**"
            ],
            "defines": [],
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "${default}"
        }
    ],
    "version": 4
})JSON";
}

// Linux/macOS build script:
//  1. Compiles every .cpp under Assets/ into Intermediate/<Config>/*.o
//  2. Links those objects + anything in Library/ into Packaging/<Config>/<name>
std::string BuildScriptSh(const std::string& projectName)
{
    std::string result = R"SH(#!/usr/bin/env bash
# Usage: ./build.sh [Development|Shipping]
set -e
CONFIG="${1:-Development}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="$ROOT/Assets"
LIB_DIR="$ROOT/Library"
INT_DIR="$ROOT/Intermediate/$CONFIG"
OUT_DIR="$ROOT/Packaging/$CONFIG"
PROJECT_NAME="__NAME__"

mkdir -p "$INT_DIR" "$OUT_DIR"

if [ "$CONFIG" == "Shipping" ]; then
    FLAGS="-O2 -DNDEBUG -std=c++17"
else
    FLAGS="-g -O0 -std=c++17"
fi

OBJS=()
while IFS= read -r -d '' src; do
    obj="$INT_DIR/$(basename "${src%.cpp}").o"
    echo "Compiling: $src"
    g++ $FLAGS -I"$SRC_DIR" -I"$LIB_DIR" -c "$src" -o "$obj"
    OBJS+=("$obj")
done < <(find "$SRC_DIR" -name "*.cpp" -print0)

if [ ${#OBJS[@]} -eq 0 ]; then
    echo "No .cpp files found under Assets/"
    exit 1
fi

# Each library lives in its own Library/<Name>/ folder. Add every such
# folder to the linker search path, and auto-link ones that actually have
# a compiled binary in them (skips empty placeholders left by "Add Library").
LFLAGS=("-L$LIB_DIR")
LINKFLAGS=()
if [ -d "$LIB_DIR" ]; then
    for sub in "$LIB_DIR"/*/; do
        [ -d "$sub" ] || continue
        libname="$(basename "$sub")"
        LFLAGS+=("-L$sub")
        if [ -s "$sub/$libname.lib" ] || [ -s "$sub/lib$libname.a" ] || [ -s "$sub/lib$libname.so" ]; then
            LINKFLAGS+=("-l$libname")
        fi
    done
fi

echo "Linking: $PROJECT_NAME ($CONFIG)"
g++ $FLAGS "${OBJS[@]}" "${LFLAGS[@]}" "${LINKFLAGS[@]}" -o "$OUT_DIR/$PROJECT_NAME"
echo "Build complete: $OUT_DIR/$PROJECT_NAME"
)SH";
    size_t pos = result.find("__NAME__");
    result.replace(pos, 8, projectName);
    return result;
}

// Windows build script (assumes g++ from MinGW-w64 is on PATH; swap for
// cl.exe + a Developer Command Prompt environment if you prefer MSVC).
std::string BuildScriptBat(const std::string& projectName)
{
    std::string result = R"BAT(@echo off
setlocal enabledelayedexpansion

set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Development

set ROOT=%~dp0
set SRC_DIR=%ROOT%Assets
set LIB_DIR=%ROOT%Library
set INT_DIR=%ROOT%Intermediate\%CONFIG%
set OUT_DIR=%ROOT%Packaging\%CONFIG%
set PROJECT_NAME=__NAME__

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

rem Each library lives in its own Library\<Name>\ folder. Add every such
rem folder to the linker search path, and auto-link ones that actually have
rem a compiled binary in them (skips empty placeholders left by "Add Library").
set LFLAGS=-L"%LIB_DIR%"
set LINKFLAGS=
for /d %%d in ("%LIB_DIR%\*") do (
    set LFLAGS=!LFLAGS! -L"%%d"
    set LIBNAME=%%~nxd
    if exist "%%d\!LIBNAME!.lib" (
        for %%s in ("%%d\!LIBNAME!.lib") do if %%~zs GTR 0 set LINKFLAGS=!LINKFLAGS! -l!LIBNAME!
    )
)

echo Linking: %PROJECT_NAME% (%CONFIG%)
g++ %FLAGS% !OBJS! !LFLAGS! !LINKFLAGS! -o "%OUT_DIR%\%PROJECT_NAME%.exe"
echo Build complete: %OUT_DIR%\%PROJECT_NAME%.exe
)BAT";
    size_t pos = result.find("__NAME__");
    result.replace(pos, 8, projectName);
    return result;
}

std::string MainCpp(const std::string& projectName)
{
    std::string result = R"CPP(#include <iostream>

// __NAME__ entry point.
// Add your headers/sources under Assets/ - the build picks up every
// .cpp file found there automatically.
int main()
{
    std::cout << "__NAME__ running." << std::endl;
    return 0;
}
)CPP";
    size_t pos;
    while ((pos = result.find("__NAME__")) != std::string::npos)
        result.replace(pos, 8, projectName);
    return result;
}

std::string ConfigIni(const std::string& projectName)
{
    std::string result = R"INI([Project]
Name=__NAME__
Version=0.1.0

[Build]
DefaultConfig=Development
)INI";
    size_t pos = result.find("__NAME__");
    result.replace(pos, 8, projectName);
    return result;
}

std::string ReadmeMd(const std::string& projectName)
{
    std::string result = R"MD(# __NAME__

Generated by CppProjectManager.

## Folder layout

- `Assets/`        Your source code and headers.
- `Config/`         `.ini` / config files used at runtime.
- `Intermediate/`   Compiler-generated `.o`/debug output (safe to delete).
- `Library/`        Third-party headers, `.lib`/`.dll`/`.so` files to link against.
- `Packaging/Development/`  Debug build output (exe/dll).
- `Packaging/Shipping/`     Release build output (exe/dll).

## Building

Open this folder in VS Code and run the build task (Ctrl+Shift+B / Cmd+Shift+B),
or from a terminal:

```
./build.sh Development   # or Shipping   (Linux/macOS)
build.bat Development    # or Shipping   (Windows)
```

Output binaries land in `Packaging/Development/` or `Packaging/Shipping/`.
Object files land in `Intermediate/<Config>/`.

Add any static/import libraries you link against to `Library/`, and drop
their headers there too so `#include` resolves for both the compiler and
VS Code IntelliSense.
)MD";
    size_t pos;
    while ((pos = result.find("__NAME__")) != std::string::npos)
        result.replace(pos, 8, projectName);
    return result;
}

std::string GitIgnore()
{
    return R"GI(Intermediate/
Packaging/
*.o
*.obj
*.pdb
)GI";
}

// Per-library header, e.g. Library/Zlib/Zlib.h. Declare that library's
// public API here so consumers can #include "Library.h" (the umbrella
// header) and get everything at once.
std::string LibraryHeaderStub(const std::string& libraryName)
{
    std::string result = R"CPP(#pragma once
// __NAME__
// Declare the public API this library exposes (functions, classes, etc.).
//
// Drop the real compiled binaries in this same folder, replacing the
// placeholders that were generated here:
//   __NAME__.lib   (Windows import/static library)
//   __NAME__.dll   (Windows runtime library)
// On Linux/macOS, add lib__NAME__.a / lib__NAME__.so instead if needed.
)CPP";
    size_t pos;
    while ((pos = result.find("__NAME__")) != std::string::npos)
        result.replace(pos, 8, libraryName);
    return result;
}

} // namespace Templates
