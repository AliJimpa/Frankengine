// ============================================================
//  VSCode C++ Project Manager
//
//  A small console tool that:
//    1) Creates a new C++ project (folder structure + files
//       + a .bat file to compile with g++) at an address
//       (path) you give it, then opens it in VS Code.
//    2) Opens an existing project folder in VS Code.
//
//  Requirements:
//    - MinGW-w64 (g++) installed and on PATH        -> for compiling
//    - VS Code installed with the "code" command
//      added to PATH (VS Code > Command Palette >
//      "Shell Command: Install 'code' command in PATH")
//
//  Build (Windows, from a Developer/MinGW terminal):
//      g++ -std=c++17 main.cpp -o ProjectManager.exe
//
//  This program itself uses <filesystem>, so it needs a
//  C++17-capable compiler.
// ============================================================

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <limits>

namespace fs = std::filesystem;

// ------------------------------------------------------------
// Small helpers
// ------------------------------------------------------------

// Reads a full line of input, trims leading/trailing whitespace
// and surrounding quotes (handy when a path is pasted with quotes).
static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);

    // trim whitespace
    size_t start = line.find_first_not_of(" \t\r\n");
    size_t end   = line.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    line = line.substr(start, end - start + 1);

    // strip surrounding quotes if present
    if (line.size() >= 2 && line.front() == '"' && line.back() == '"') {
        line = line.substr(1, line.size() - 2);
    }
    return line;
}

static void writeFile(const fs::path& path, const std::string& content) {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        std::cerr << "  ! Failed to write: " << path << "\n";
        return;
    }
    out << content;
}

// Opens a folder in VS Code using the `code` CLI.
static void openInVSCode(const fs::path& folder) {
    std::string cmd = "code \"" + folder.string() + "\"";
    int result = std::system(cmd.c_str());
    if (result != 0) {
        std::cout << "\n  Note: could not launch VS Code automatically.\n"
                     "  Make sure the 'code' command is on your PATH\n"
                     "  (VS Code -> Ctrl+Shift+P -> 'Shell Command: Install "
                     "\"code\" command in PATH'),\n"
                     "  or open this folder manually:\n    "
                  << folder.string() << "\n";
    } else {
        std::cout << "  Opened in VS Code: " << folder.string() << "\n";
    }
}

// ------------------------------------------------------------
// Project scaffolding
// ------------------------------------------------------------

static const std::string kMainCppTemplate =
R"CPP(#include <iostream>

int main() {
    std::cout << "Hello from PROJECT_NAME!" << std::endl;
    return 0;
}
)CPP";

// The compile .bat file. Produces build\ProjectName.exe from
// everything in src\*.cpp, using g++ (MinGW-w64).
static std::string makeCompileBat(const std::string& projectName) {
    return
"@echo off\r\n"
"REM ============================================================\r\n"
"REM  compile.bat - builds " + projectName + "\r\n"
"REM  Requires g++ (MinGW-w64) on PATH.\r\n"
"REM ============================================================\r\n"
"setlocal\r\n"
"if not exist build mkdir build\r\n"
"echo Compiling " + projectName + " ...\r\n"
"g++ -std=c++17 -Wall -Iinclude src\\*.cpp -o build\\" + projectName + ".exe\r\n"
"if %ERRORLEVEL% NEQ 0 (\r\n"
"    echo.\r\n"
"    echo Build FAILED.\r\n"
"    exit /b %ERRORLEVEL%\r\n"
") else (\r\n"
"    echo.\r\n"
"    echo Build succeeded: build\\" + projectName + ".exe\r\n"
")\r\n"
"endlocal\r\n";
}

// bat file that compiles AND runs the app.
static std::string makeRunBat(const std::string& projectName) {
    return
"@echo off\r\n"
"call compile.bat\r\n"
"if %ERRORLEVEL% EQU 0 (\r\n"
"    echo.\r\n"
"    echo Running " + projectName + " ...\r\n"
"    echo ------------------------------------\r\n"
"    build\\" + projectName + ".exe\r\n"
")\r\n";
}

static std::string makeTasksJson(const std::string& projectName) {
    return
R"JSON({
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build (compile.bat)",
            "type": "shell",
            "command": "${workspaceFolder}\\compile.bat",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": ["$gcc"]
        },
        {
            "label": "Build & Run (run.bat)",
            "type": "shell",
            "command": "${workspaceFolder}\\run.bat",
            "problemMatcher": ["$gcc"]
        }
    ]
}
)JSON";
}

static std::string makeLaunchJson(const std::string& projectName) {
    return std::string(
R"JSON({
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug )JSON") + projectName + R"JSON(",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}\\build\\)JSON" + projectName + R"JSON(.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": true,
            "preLaunchTask": "Build (compile.bat)",
            "MIMode": "gdb"
        }
    ]
}
)JSON";
}

static std::string makeGitignore() {
    return
"build/\n"
"*.exe\n"
"*.o\n"
"*.obj\n"
".vscode/ipch/\n";
}

static std::string makeReadme(const std::string& projectName) {
    return
"# " + projectName + "\n\n"
"## Structure\n"
"- `src/`      - .cpp source files\n"
"- `include/`  - header files\n"
"- `build/`    - compiled output (created automatically)\n"
"- `compile.bat` - compiles the project with g++\n"
"- `run.bat`     - compiles then runs the project\n\n"
"## Build\n"
"Double-click `compile.bat`, or in VS Code press Ctrl+Shift+B.\n\n"
"## Run\n"
"Double-click `run.bat`, or run the exe in `build\\` after building.\n";
}

// Creates the full project structure at basePath/projectName
static bool createProject(const fs::path& basePath, const std::string& projectName) {
    fs::path root = basePath / projectName;

    if (fs::exists(root)) {
        std::cout << "  ! Folder already exists: " << root.string() << "\n"
                      "    Choose a different name/location, or open it instead.\n";
        return false;
    }

    std::error_code ec;
    fs::create_directories(root / "src", ec);
    fs::create_directories(root / "include", ec);
    fs::create_directories(root / "build", ec);
    fs::create_directories(root / ".vscode", ec);

    if (ec) {
        std::cerr << "  ! Failed to create folders: " << ec.message() << "\n";
        return false;
    }

    // main.cpp with project name substituted in
    std::string mainCpp = kMainCppTemplate;
    size_t pos = mainCpp.find("PROJECT_NAME");
    if (pos != std::string::npos) mainCpp.replace(pos, 12, projectName);

    writeFile(root / "src" / "main.cpp", mainCpp);
    writeFile(root / "compile.bat", makeCompileBat(projectName));
    writeFile(root / "run.bat", makeRunBat(projectName));
    writeFile(root / ".vscode" / "tasks.json", makeTasksJson(projectName));
    writeFile(root / ".vscode" / "launch.json", makeLaunchJson(projectName));
    writeFile(root / ".gitignore", makeGitignore());
    writeFile(root / "README.md", makeReadme(projectName));

    std::cout << "\n  Project created at: " << root.string() << "\n"
                 "    src/main.cpp\n"
                 "    include/\n"
                 "    build/\n"
                 "    .vscode/tasks.json, launch.json\n"
                 "    compile.bat, run.bat\n"
                 "    README.md, .gitignore\n";

    openInVSCode(root);
    return true;
}

// ------------------------------------------------------------
// Menu
// ------------------------------------------------------------

static void printMenu() {
    std::cout << "\n============================================\n"
                 " VSCode C++ Project Manager\n"
                 "============================================\n"
                 " 1) Create new project\n"
                 " 2) Open existing project\n"
                 " 3) Exit\n"
                 "--------------------------------------------\n"
                 "Choice: ";
}

int main() {
    std::cout << "Welcome to the VSCode C++ Project Manager.\n";

    while (true) {
        printMenu();
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            std::string address = readLine(
                "\nEnter the address (folder path) where the project should "
                "be created,\ne.g. C:\\Users\\You\\Projects : ");
            if (address.empty()) {
                std::cout << "  ! No address given.\n";
                continue;
            }

            std::error_code ec;
            fs::create_directories(address, ec); // make sure base path exists
            if (ec) {
                std::cout << "  ! Could not access/create that path: "
                          << ec.message() << "\n";
                continue;
            }

            std::string name = readLine("Enter the project name: ");
            if (name.empty()) {
                std::cout << "  ! No project name given.\n";
                continue;
            }

            createProject(fs::path(address), name);
        }
        else if (choice == "2") {
            std::string address = readLine(
                "\nEnter the address (folder path) of the project to open: ");
            if (address.empty()) {
                std::cout << "  ! No address given.\n";
                continue;
            }
            if (!fs::exists(address)) {
                std::cout << "  ! That path does not exist: " << address << "\n";
                continue;
            }
            openInVSCode(fs::path(address));
        }
        else if (choice == "3") {
            std::cout << "Goodbye.\n";
            break;
        }
        else {
            std::cout << "  ! Please enter 1, 2, or 3.\n";
        }
    }

    return 0;
}