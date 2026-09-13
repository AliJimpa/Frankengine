#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include "IGameModule.h"

// ============================================================
//  Runtime.exe
//
//  Links Engine.lib statically (GetEngineAPI() is a normal call).
//  Every other *.dll sitting next to this exe that exports
//  createModule() is treated as a plugin: loaded, started, and
//  updated every frame alongside any others found.
// ============================================================

struct LoadedModule
{
    HMODULE dll;
    IGameModule *instance;
    std::string fileName;
};

static std::vector<LoadedModule> LoadAllModules(EngineAPI *api)
{
    std::vector<LoadedModule> modules;

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA("*.dll", &findData);
    if (hFind == INVALID_HANDLE_VALUE)
        return modules;

    do
    {
        HMODULE dll = LoadLibraryA(findData.cFileName);
        if (!dll)
            continue;

        auto create = (CreateModuleFunc)GetProcAddress(dll, "createModule");
        if (!create)
        {
            // Not every dll beside the exe is necessarily a game module
            // (could be an unrelated runtime dependency) - skip quietly.
            FreeLibrary(dll);
            continue;
        }

        IGameModule *instance = create();
        if (!instance)
        {
            FreeLibrary(dll);
            continue;
        }

        std::cout << "Loaded module: " << findData.cFileName << "\n";
        instance->start(api);
        modules.push_back({dll, instance, findData.cFileName});

    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
    return modules;
}

static void UnloadAllModules(std::vector<LoadedModule> &modules)
{
    for (auto &m : modules)
    {
        m.instance->shutdown();
        delete m.instance;
        FreeLibrary(m.dll);
    }
    modules.clear();
}

int main()
{
    EngineAPI *api = GetEngineAPI();

    std::vector<LoadedModule> modules = LoadAllModules(api);
    if (modules.empty())
    {
        std::cout << "No game modules found (no *.dll beside Runtime.exe exports createModule())\n";
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    while (true)
    {
        for (auto &m : modules)
            m.instance->update();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    UnloadAllModules(modules); // unreachable for now - no shutdown signal yet
    return 0;
}