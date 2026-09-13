#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

struct ProjectEntry
{
    std::string name;
    fs::path path;
};

// Persists the list of known projects to a small text file in the user's
// home directory (~/.cppprojectmanager/projects.txt) so they can be
// re-opened across runs of the manager app.
class Registry
{
public:
    static std::vector<ProjectEntry> Load();
    static void Add(const ProjectEntry& entry);

private:
    static fs::path RegistryFile();
};
