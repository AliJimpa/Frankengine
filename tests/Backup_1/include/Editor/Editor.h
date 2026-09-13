#pragma once

void Initiation()
{
    MyUI = new UserInterface(this);
}

class App
{
public:
    App()
    {
        MyUI = new UserInterface(this);
        run();
    }
    EditorGUISystem *GetUI() const { return MyUI; }
    void Quit()
    {
        bIsRun = false;
    }

protected:
    virtual void OnStart() {};
    virtual void OnLateUpdate() {};
    virtual void OnUpdate() {};
    virtual void OnEnd() {};
    virtual bool CanLoop() const
    {
        return GetUI()->canRender();
    }

private:
    void run()
    {
        // Start Applications
        OnStart();

        // Init EditorGUISystem
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
    EditorGUISystem *MyUI;
    bool bIsRun = true;
};

class Editor : public App
{
    void OnStart() override {};
    void OnLateUpdate() override {};
    void OnUpdate() override {};
    void OnEnd() override {};
    bool CanLoop() const override { return App::CanLoop(); };
};