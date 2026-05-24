#include "Engine.h"
#include "EditorGUISystem.h"
#include <vector>
#include <memory>
#include "W_HelloWorld.h"
#include "W_TEST.h"
#include "W_Demo.h"

int main()
{
    std::vector<std::shared_ptr<ISubsystem>> allSubsystems;

    // Register
    std::shared_ptr<EditorGUISystem> GUI = std::make_shared<EditorGUISystem>();
    allSubsystems.push_back(GUI);

    GUI->CreateWidget<W_HelloWorld>();
    GUI->CreateWidget<W_TEST>();
    GUI->CreateWidget<W_Demo>();

    for (auto &system : allSubsystems)
        system->Initialize();

    while (running)
    {
        for (auto &system : allSubsystems)
            system->Update();
    }

    for (auto &system : allSubsystems)
        system->Shutdown();

    return 0;
}

//
// void RegisterSystem(std::shared_ptr<EditorGUISystem> gui)
// {
//     allSubsystems.push_back(gui); // Holds shared ownership
//     guiSystems.push_back(gui);    // Holds shared ownership
// }