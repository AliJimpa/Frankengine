#include "ProjectManager.h"
#include "Registry.h"
#include <iostream>
#include <limits>

static void PrintMenu()
{
    std::cout << "\n=== C++ Project Manager ===\n"
              << "1. Create new project\n"
              << "2. Open existing project (known list)\n"
              << "3. Open a project by path\n"
              << "4. List known projects\n"
              << "5. Add library to a project\n"
              << "6. Exit\n"
              << "Choose an option: ";
}

static std::string PromptLine(const std::string& prompt)
{
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

static void HandleCreate()
{
    std::string name = PromptLine("Project name: ");
    std::string parent = PromptLine("Parent directory (where the project folder will be created): ");

    if (parent.empty())
        parent = fs::current_path().string();

    fs::path created = ProjectManager::CreateProject(name, parent);
    if (created.empty())
        return;

    Registry::Add({name, created});

    std::string openNow = PromptLine("Open in VS Code now? [Y/n]: ");
    if (openNow.empty() || openNow == "y" || openNow == "Y")
        ProjectManager::OpenInVSCode(created);
}

static void HandleOpenFromList()
{
    auto projects = Registry::Load();
    if (projects.empty())
    {
        std::cout << "No known projects yet. Create one first.\n";
        return;
    }

    for (size_t i = 0; i < projects.size(); ++i)
        std::cout << "  " << (i + 1) << ". " << projects[i].name
                  << "  (" << projects[i].path.string() << ")\n";

    std::string choice = PromptLine("Select a project number: ");
    try
    {
        int idx = std::stoi(choice) - 1;
        if (idx < 0 || idx >= static_cast<int>(projects.size()))
        {
            std::cout << "Invalid selection.\n";
            return;
        }
        ProjectManager::OpenInVSCode(projects[idx].path);
    }
    catch (...)
    {
        std::cout << "Invalid input.\n";
    }
}

static void HandleOpenByPath()
{
    std::string path = PromptLine("Full path to project folder: ");
    ProjectManager::OpenInVSCode(path);
}

static void HandleList()
{
    auto projects = Registry::Load();
    if (projects.empty())
    {
        std::cout << "No known projects yet.\n";
        return;
    }
    for (auto& p : projects)
        std::cout << "  - " << p.name << "  (" << p.path.string() << ")\n";
}

static void HandleAddLibrary()
{
    auto projects = Registry::Load();
    fs::path projectPath;

    if (!projects.empty())
    {
        std::cout << "Known projects:\n";
        for (size_t i = 0; i < projects.size(); ++i)
            std::cout << "  " << (i + 1) << ". " << projects[i].name
                      << "  (" << projects[i].path.string() << ")\n";
        std::cout << "  0. Enter a path manually\n";
    }

    std::string choice = PromptLine(projects.empty()
        ? "Project folder path: "
        : "Select a project (or 0 for manual path): ");

    if (projects.empty())
    {
        projectPath = choice;
    }
    else
    {
        try
        {
            int idx = std::stoi(choice);
            if (idx == 0)
                projectPath = PromptLine("Project folder path: ");
            else if (idx >= 1 && idx <= static_cast<int>(projects.size()))
                projectPath = projects[idx - 1].path;
            else
            {
                std::cout << "Invalid selection.\n";
                return;
            }
        }
        catch (...)
        {
            std::cout << "Invalid input.\n";
            return;
        }
    }

    if (!fs::exists(projectPath) || !fs::exists(projectPath / "Library"))
    {
        std::cout << "That doesn't look like a project created by this tool "
                     "(no Library/ folder found at " << projectPath << ").\n";
        return;
    }

    std::string libName = PromptLine("Library name (e.g. Zlib, Boost, MyPhysicsSDK): ");
    ProjectManager::AddLibrary(projectPath, libName);
}

int main()
{
    while (true)
    {
        PrintMenu();
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1")
            HandleCreate();
        else if (choice == "2")
            HandleOpenFromList();
        else if (choice == "3")
            HandleOpenByPath();
        else if (choice == "4")
            HandleList();
        else if (choice == "5")
            HandleAddLibrary();
        else if (choice == "6" || choice.empty())
            break;
        else
            std::cout << "Unknown option.\n";
    }

    std::cout << "Goodbye.\n";
    return 0;
}
