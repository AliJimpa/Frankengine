#pragma once
#include <vector>
#include <memory>
#include "ISubsystem.h"
#include "DeltaTimer.h"

class SystemManager
{
private:
    std::vector<std::unique_ptr<ISubsystem>> modules;
    bool running = false;

public:
    void RegisterSystem(std::unique_ptr<ISubsystem> mod)
    {
        modules.push_back(std::move(mod));
    }

    void Run()
    {
        DeltaTimer timer;

        // Initialization Phase
        for (auto &mod : modules)
            mod->Initialize();

        running = true;

        // Main Loop
        while (running)
        {
            // Calculate the elapsed time since the last loop iteration
            float dt = timer.getDeltaTime();

            // Update all modules with the real elapsed time
            for (auto &mod : modules)
            {
                mod->Update(dt);
            }
        }

        // Shutdown Phase
        for (auto &mod : modules)
            mod->Shutdown();
    }

    void Shutdown()
    {
        running = false;
    }
};
