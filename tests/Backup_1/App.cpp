
#include "App.h"
#include "UserInterface.h"

App::App()
{
    MyUI = new UserInterface(this);
    run();
}

bool App::CanLoop() const
{
    return GetUI()->canRender();
}

void App::run()
{
    // Start Applications
    OnStart();

    // Init GUI
    if (!GetUI()->init())
        return;

    // Main loop
    while (bIsRun && CanLoop())
    {
        OnLateUpdate();
        GetUI()->render();
        OnUpdate();
    }

    GetUI()->shutdown(); // Cleanup
    OnEnd();
}

void App::Quit()
{
    bIsRun = false;
}
