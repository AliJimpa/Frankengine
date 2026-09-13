#pragma once

#include "Editor/system/EditorGUISystem.h"
#include "Engine/core/Application.h"

// ============================================================
//  App - owns the main loop and the EditorGUISystem.
//
//  Fixed vs. the first draft:
//   - removed a stray free function that referenced MyUI/this
//     outside of any class (didn't compile)
//   - App::run() now calls Initialize()/Update()/Shutdown() to
//     match ISubsystem, instead of init()/render()/shutdown()
//     which didn't exist
// ============================================================
class App
{
public:
    App() : MyUI(new EditorGUISystem()) {}
    virtual ~App() { delete MyUI; }

    EditorGUISystem* GetUI() const { return MyUI; }
    void Quit() { bIsRun = false; }

    void Run()
    {
        OnStart();
        MyUI->Initialize();

        while (bIsRun && CanLoop())
        {
            OnLateUpdate();
            MyUI->Update();
            OnUpdate();
        }

        MyUI->Shutdown();
        OnEnd();
    }

protected:
    virtual void OnStart() {}
    virtual void OnLateUpdate() {}
    virtual void OnUpdate() {}
    virtual void OnEnd() {}
    virtual bool CanLoop() const { return Application::isRunning(); }

private:
    EditorGUISystem* MyUI;
    bool bIsRun = true;
};

class Editor : public App
{
protected:
    void OnStart() override {}
    void OnLateUpdate() override {}
    void OnUpdate() override {}
    void OnEnd() override {}
};
