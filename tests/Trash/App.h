#pragma once

#include "UserInterface.h"

class App
{
public:
    App();
    GUI *GetUI() const { return MyUI; }
    void Quit();

protected:
    virtual void OnStart() {};
    virtual void OnLateUpdate() {};
    virtual void OnUpdate() {};
    virtual void OnEnd() {};
    virtual bool CanLoop() const;

private:
    void run();
    GUI *MyUI;
    bool bIsRun = true;
};
