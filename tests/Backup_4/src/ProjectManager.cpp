#include "ProjectManager.h"
#include "TemplateFiles.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>

void ProjectManager::WriteTextFile(const fs::path& file, const std::string& content)
{
    std::ofstream out(file, std::ios::binary);
    out << content;
}

void ProjectManager::CreateFolderStructure(const fs::path& root)
{
    fs::create_directories(root / "Assets");
    fs::create_directories(root / "Config");
    fs::create_directories(root / "Intermediate" / "Development");
    fs::create_directories(root / "Intermediate" / "Shipping");
    fs::create_directories(root / "Library");
    fs::create_directories(root / "Packaging" / "Development");
    fs::create_directories(root / "Packaging" / "Shipping");
    fs::create_directories(root / ".vscode");
}

void ProjectManager::WriteVSCodeConfig(const fs::path& root, const std::string& projectName)
{
    fs::path vscode = root / ".vscode";
    WriteTextFile(vscode / "tasks.json", Templates::TasksJson());
    WriteTextFile(vscode / "launch.json", Templates::LaunchJson(projectName));
    WriteTextFile(vscode / "c_cpp_properties.json", Templates::CppPropertiesJson());
}

void ProjectManager::WriteBuildScripts(const fs::path& root, const std::string& projectName)
{
    fs::path sh = root / "build.sh";
    WriteTextFile(sh, Templates::BuildScriptSh(projectName));
    WriteTextFile(root / "build.bat", Templates::BuildScriptBat(projectName));

#ifndef _WIN32
    fs::permissions(sh, fs::perms::owner_all | fs::perms::group_read |
                             fs::perms::group_exec | fs::perms::others_read |
                             fs::perms::others_exec,
                     fs::perm_options::add);
#endif
}

void ProjectManager::WriteStubSources(const fs::path& root, const std::string& projectName)
{
    WriteTextFile(root / "Assets" / "main.cpp", Templates::MainCpp(projectName));
    WriteTextFile(root / "Config" / "config.ini", Templates::ConfigIni(projectName));
    WriteTextFile(root / "README.md", Templates::ReadmeMd(projectName));
    WriteTextFile(root / ".gitignore", Templates::GitIgnore());

    // Library/Library.h is the umbrella header - regenerated (empty, with
    // a note) now, and again every time a library is added via AddLibrary.
    RegenerateLibraryHeader(root);
}

fs::path ProjectManager::CreateProject(const std::string& projectName, const fs::path& parentDir)
{
    if (projectName.empty())
    {
        std::cerr << "Project name cannot be empty.\n";
        return {};
    }

    fs::path root = parentDir / projectName;

    if (fs::exists(root))
    {
        std::cerr << "Error: " << root << " already exists.\n";
        return {};
    }

    try
    {
        CreateFolderStructure(root);
        WriteVSCodeConfig(root, projectName);
        WriteBuildScripts(root, projectName);
        WriteStubSources(root, projectName);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to create project: " << e.what() << "\n";
        return {};
    }

    std::cout << "Created project at: " << root << "\n";
    return root;
}

bool ProjectManager::AddLibrary(const fs::path& projectRoot, const std::string& libraryName)
{
    if (libraryName.empty())
    {
        std::cerr << "Library name cannot be empty.\n";
        return false;
    }

    fs::path libDir = projectRoot / "Library" / libraryName;
    if (fs::exists(libDir))
    {
        std::cerr << "Error: library already exists at " << libDir << "\n";
        return false;
    }

    try
    {
        fs::create_directories(libDir);
        WriteTextFile(libDir / (libraryName + ".h"), Templates::LibraryHeaderStub(libraryName));

        // 0-byte placeholders establishing the expected filenames. Overwrite
        // these with the real compiled binaries; the build scripts only
        // auto-link a library once its .lib/.a/.so file is non-empty.
        std::ofstream(libDir / (libraryName + ".lib"), std::ios::binary).close();
        std::ofstream(libDir / (libraryName + ".dll"), std::ios::binary).close();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to add library: " << e.what() << "\n";
        return false;
    }

    RegenerateLibraryHeader(projectRoot);
    std::cout << "Added library: " << libDir << "\n";
    return true;
}

void ProjectManager::RegenerateLibraryHeader(const fs::path& projectRoot)
{
    fs::path libDir = projectRoot / "Library";
    if (!fs::exists(libDir))
        return;

    std::vector<std::string> includes;
    for (auto& entry : fs::directory_iterator(libDir))
    {
        if (!entry.is_directory())
            continue;
        std::string name = entry.path().filename().string();
        fs::path header = entry.path() / (name + ".h");
        if (fs::exists(header))
            includes.push_back(name + "/" + name + ".h");
    }
    std::sort(includes.begin(), includes.end());

    std::ostringstream out;
    out << "#pragma once\n"
        << "// Auto-generated by CppProjectManager.\n"
        << "// Aggregates every library header found under Library/<Name>/<Name>.h.\n"
        << "// Regenerated each time a library is added - manual edits here may be\n"
        << "// overwritten. #include \"Library.h\" from your Assets code to pull in\n"
        << "// every library's public API at once.\n\n";

    if (includes.empty())
    {
        out << "// No libraries added yet. Use the manager tool's \"Add library to a\n"
            << "// project\" option, or manually add a Library/<Name>/<Name>.h yourself.\n";
    }
    else
    {
        for (auto& inc : includes)
            out << "#include \"" << inc << "\"\n";
    }

    WriteTextFile(libDir / "Library.h", out.str());
}

bool ProjectManager::OpenInVSCode(const fs::path& projectPath)
{
    if (!fs::exists(projectPath))
    {
        std::cerr << "Path does not exist: " << projectPath << "\n";
        return false;
    }

    std::string cmd = "code \"" + projectPath.string() + "\"";
    int result = std::system(cmd.c_str());

    if (result != 0)
    {
        std::cerr << "Could not launch VS Code automatically (is the `code` "
                     "command on your PATH?). You can open it manually:\n  "
                  << projectPath << "\n";
        return false;
    }
    return true;
}
