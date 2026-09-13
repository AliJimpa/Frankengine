// EditorConsole.cpp
//
// Minimal console Editor: create / open / compile / play.
// No ImGui, no GLFW - pure filesystem + process orchestration.
// Run this from the engine root (the folder containing sdk/,
// engine/, runtime/, projects/, build/).

#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// The prebuilt Runtime.exe every new project gets its own copy of.
static const fs::path RUNTIME_TEMPLATE = "build/Debug/bin/Runtime/Runtime.exe";

struct Project
{
    bool isOpen = false;
    std::string name;
    fs::path root; // .../<ProjectName>
};

static Project g_project;

// ---------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------
static std::string Prompt(const std::string &label)
{
    std::cout << label;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

static void WriteFile(const fs::path &path, const std::string &content)
{
    std::ofstream out(path, std::ios::binary);
    out << content;
}

static std::string ReplaceAll(std::string text, const std::string &from, const std::string &to)
{
    size_t pos = 0;
    while ((pos = text.find(from, pos)) != std::string::npos)
    {
        text.replace(pos, from.length(), to);
        pos += to.length();
    }
    return text;
}

// Runs a command and blocks until it finishes. Used for Compile.
static bool RunProcessAndWait(const std::string &commandLine)
{
    STARTUPINFOA si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    std::string cmd = commandLine; // CreateProcess needs a writable buffer

    if (!CreateProcessA(nullptr, cmd.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi))
    {
        std::cout << "[Editor] Failed to launch: " << commandLine << "\n";
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return exitCode == 0;
}

// Runs a command and returns immediately. Used for Play.
static bool RunProcessDetached(const std::string &commandLine, const fs::path &workingDir)
{
    STARTUPINFOA si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    std::string cmd = commandLine;
    std::string cwd = workingDir.string();

    if (!CreateProcessA(nullptr, cmd.data(), nullptr, nullptr, FALSE,
                        CREATE_NEW_CONSOLE, nullptr, cwd.c_str(), &si, &pi))
    {
        std::cout << "[Editor] Failed to launch: " << commandLine << "\n";
        return false;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

// ---------------------------------------------------------------
// Templates seeded into a new project. {NAME} -> project name.
// Same shape as SampleGame - its own per-project API macro.
// ---------------------------------------------------------------
static const char *API_HEADER_TEMPLATE = R"(#pragma once

#ifdef {NAME}_EXPORTS
    #define {NAME}_API __declspec(dllexport)
#else
    #define {NAME}_API __declspec(dllimport)
#endif
)";

static const char *MODULE_CPP_TEMPLATE = R"(#define {NAME}_EXPORTS
#include "{NAME}API.h"
#include "IGameModule.h"

class {NAME}_API {NAME} : public IGameModule
{
public:
    void start(EngineAPI* api) override
    {
        m_api = api;
        if (m_api && m_api->logger)
            m_api->logger->log("{NAME} started!");
    }

    void update() override
    {
        if (m_api && m_api->logger)
            m_api->logger->log("{NAME} update tick");
    }

    void shutdown() override
    {
        if (m_api && m_api->logger)
            m_api->logger->log("{NAME} shutting down");
    }

private:
    EngineAPI* m_api = nullptr;
};

extern "C" {NAME}_API IGameModule* createModule()
{
    return new {NAME}();
}
)";

static const char *BUILD_BAT_TEMPLATE = R"(@echo off
setlocal

set PROJECT_DIR=%~dp0
for %%I in ("%PROJECT_DIR%.") do set PROJECT_NAME=%%~nI

set SDK_INCLUDE=%PROJECT_DIR%..\..\sdk\include
set SOURCE_DIR=%PROJECT_DIR%Source
set BIN_DIR=%PROJECT_DIR%bin
set OBJ_DIR=%PROJECT_DIR%obj

if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"
if not exist "%OBJ_DIR%" mkdir "%OBJ_DIR%"

where cl.exe >nul 2>nul
if errorlevel 1 (
    echo [build.bat] cl.exe not found on PATH.
    echo [build.bat] Run this from a "Developer Command Prompt for VS 2022".
    exit /b 1
)

echo [build.bat] Building %PROJECT_NAME%.dll ...

cl.exe /EHsc /Zi /MDd /FS /std:c++17 /LD ^
    /I "%SDK_INCLUDE%" ^
    /Fo:"%OBJ_DIR%\" ^
    /Fd:"%OBJ_DIR%\%PROJECT_NAME%.pdb" ^
    /Fe:"%BIN_DIR%\%PROJECT_NAME%.dll" ^
    "%SOURCE_DIR%\*.cpp"

if errorlevel 1 (
    echo [build.bat] Build FAILED.
    exit /b 1
)

echo [build.bat] Build OK -^> %BIN_DIR%\%PROJECT_NAME%.dll
exit /b 0
)";

// ---------------------------------------------------------------
// Commands
// ---------------------------------------------------------------
static void CmdCreate()
{
    std::string name = Prompt("Project name: ");
    if (name.empty())
    {
        std::cout << "[Editor] Project name can't be empty.\n";
        return;
    }

    std::string address = Prompt("Create in folder (blank = projects/): ");
    fs::path parent = address.empty() ? fs::path("projects") : fs::path(address);
    fs::path root = parent / name;

    if (fs::exists(root))
    {
        std::cout << "[Editor] " << root << " already exists.\n";
        return;
    }

    fs::create_directories(root / "Assets");
    fs::create_directories(root / "Source");
    fs::create_directories(root / "bin");

    WriteFile(root / "Source" / (name + "API.h"), ReplaceAll(API_HEADER_TEMPLATE, "{NAME}", name));
    WriteFile(root / "Source" / (name + ".cpp"), ReplaceAll(MODULE_CPP_TEMPLATE, "{NAME}", name));
    WriteFile(root / "build.bat", BUILD_BAT_TEMPLATE);

    if (fs::exists(RUNTIME_TEMPLATE))
    {
        fs::copy_file(RUNTIME_TEMPLATE, root / "bin" / (name + ".exe"),
                      fs::copy_options::overwrite_existing);
    }
    else
    {
        std::cout << "[Editor] Warning: " << RUNTIME_TEMPLATE
                  << " not found - build Runtime.exe first, then re-create or copy it manually.\n";
    }

    g_project = {true, name, root};
    std::cout << "[Editor] Created project '" << name << "' at " << root << "\n";
}

static void CmdOpen()
{
    std::string address = Prompt("Project folder: ");
    fs::path root = address;

    if (!fs::exists(root) || !fs::is_directory(root))
    {
        std::cout << "[Editor] " << root << " does not exist.\n";
        return;
    }

    g_project = {true, root.filename().string(), root};
    std::cout << "[Editor] Opened project '" << g_project.name << "' at " << root << "\n";
}

static void CmdCompile()
{
    if (!g_project.isOpen)
    {
        std::cout << "[Editor] No project open. Use 'create' or 'open' first.\n";
        return;
    }

    fs::path bat = g_project.root / "build.bat";
    if (!fs::exists(bat))
    {
        std::cout << "[Editor] " << bat << " not found.\n";
        return;
    }

    std::cout << "[Editor] Compiling " << g_project.name << "...\n";
    bool ok = RunProcessAndWait("cmd.exe /c \"" + bat.string() + "\"");
    std::cout << (ok ? "[Editor] Compile succeeded.\n" : "[Editor] Compile FAILED.\n");
}

static void CmdPlay()
{
    if (!g_project.isOpen)
    {
        std::cout << "[Editor] No project open. Use 'create' or 'open' first.\n";
        return;
    }

    fs::path exePath = g_project.root / "bin" / (g_project.name + ".exe");
    if (!fs::exists(exePath))
    {
        std::cout << "[Editor] " << exePath << " not found. Did you Create/Compile first?\n";
        return;
    }

    std::cout << "[Editor] Launching " << exePath << "...\n";
    RunProcessDetached("\"" + exePath.string() + "\"", g_project.root / "bin");
}

static void PrintHelp()
{
    std::cout << "Commands:\n"
                 "  create   - new project (Assets/Source/bin + Runtime.exe copy + starter code)\n"
                 "  open     - open an existing project folder\n"
                 "  compile  - run build.bat -> bin/<Project>.dll\n"
                 "  play     - launch bin/<Project>.exe\n"
                 "  help     - show this list\n"
                 "  exit     - quit\n";
}

int main()
{
    std::cout << "FrankEngine Editor (console)\n";
    PrintHelp();

    std::string cmd;
    while (true)
    {
        std::cout << "\n[" << (g_project.isOpen ? g_project.name : "no project") << "] > ";
        if (!std::getline(std::cin, cmd))
            break;

        if (cmd == "create")
            CmdCreate();
        else if (cmd == "open")
            CmdOpen();
        else if (cmd == "compile")
            CmdCompile();
        else if (cmd == "play")
            CmdPlay();
        else if (cmd == "help")
            PrintHelp();
        else if (cmd == "exit")
            break;
        else if (!cmd.empty())
            std::cout << "[Editor] Unknown command: " << cmd << " (type 'help')\n";
    }

    return 0;
}

// #include "Editor/core/Editor.h"
// #include "Editor/UI/Widget/W_HelloWorld.h"
// #include "Editor/UI/Widget/W_TEST.h"
// #include "Editor/UI/Widget/W_Demo.h"

// int main()
// {
//     Editor editor;

//     editor.GetUI()->CreateWidget<W_HelloWorld>();
//     editor.GetUI()->CreateWidget<W_TEST>();
//     editor.GetUI()->CreateWidget<W_Demo>();

//     editor.Run();

//     return 0;
// }
