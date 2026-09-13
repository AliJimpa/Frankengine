#include "Registry.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

fs::path Registry::RegistryFile()
{
#ifdef _WIN32
    const char* home = std::getenv("USERPROFILE");
#else
    const char* home = std::getenv("HOME");
#endif
    fs::path base = home ? fs::path(home) : fs::current_path();
    fs::path dir = base / ".cppprojectmanager";
    fs::create_directories(dir);
    return dir / "projects.txt";
}

std::vector<ProjectEntry> Registry::Load()
{
    std::vector<ProjectEntry> entries;
    fs::path file = RegistryFile();
    if (!fs::exists(file))
        return entries;

    std::ifstream in(file);
    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty())
            continue;
        size_t tab = line.find('\t');
        if (tab == std::string::npos)
            continue;
        ProjectEntry e;
        e.name = line.substr(0, tab);
        e.path = line.substr(tab + 1);
        if (fs::exists(e.path)) // skip stale entries whose folder was moved/deleted
            entries.push_back(e);
    }
    return entries;
}

void Registry::Add(const ProjectEntry& entry)
{
    std::ofstream out(RegistryFile(), std::ios::app);
    out << entry.name << '\t' << entry.path.string() << '\n';
}
