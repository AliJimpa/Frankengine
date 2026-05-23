#pragma once

#include "UserInterface.h"

class App
{
public:
    App();
    Graphic *GetUI() const { return MyUI; }
    void Quit();

protected:
    virtual void OnStart() {};
    virtual void OnLateUpdate() {};
    virtual void OnUpdate() {};
    virtual void OnEnd() {};
    virtual bool CanLoop() const;

private:
    void run();
    Graphic *MyUI;
    bool bIsRun = true;
};
