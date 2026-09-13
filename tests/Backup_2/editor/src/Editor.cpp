#include <iostream>
#include "Editor/system/EditorGUISystem.h"
#include "Editor/UI/Widget/W_HelloWorld.h"
#include "Editor/UI/Widget/W_TEST.h"
#include "Editor/UI/Widget/W_Demo.h"

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

    while (Application::isRunning())
    {
        for (auto &system : allSubsystems)
            system->Update();
    }

    for (auto &system : allSubsystems)
        system->Shutdown();

    return 0;
}