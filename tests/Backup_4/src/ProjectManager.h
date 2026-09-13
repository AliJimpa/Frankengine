#pragma once
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

// Handles creating the on-disk folder structure for a new C++ project,
// generating its VS Code configuration (tasks.json, launch.json,
// c_cpp_properties.json), generating cross-platform build scripts,
// and opening projects in VS Code.
class ProjectManager
{
public:
    // Creates <parentDir>/<projectName> with the full folder structure,
    // VS Code config, build scripts and stub source files.
    // Returns the full path to the created project, or empty path on failure.
    static fs::path CreateProject(const std::string& projectName, const fs::path& parentDir);

    // Opens an existing project folder in VS Code (requires the `code`
    // CLI command to be on PATH).
    static bool OpenInVSCode(const fs::path& projectPath);

    // Adds Library/<libraryName>/ containing <libraryName>.h (stub) plus
    // .lib/.dll placeholders, then regenerates the umbrella Library.h.
    static bool AddLibrary(const fs::path& projectRoot, const std::string& libraryName);

    // Rebuilds Library/Library.h by scanning Library/<Name>/<Name>.h for
    // every subfolder found directly under Library/.
    static void RegenerateLibraryHeader(const fs::path& projectRoot);

private:
    static void CreateFolderStructure(const fs::path& root);
    static void WriteVSCodeConfig(const fs::path& root, const std::string& projectName);
    static void WriteBuildScripts(const fs::path& root, const std::string& projectName);
    static void WriteStubSources(const fs::path& root, const std::string& projectName);
    static void WriteTextFile(const fs::path& file, const std::string& content);
};
