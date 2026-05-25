#pragma once

#include "ISubsystem.h"
#include <GLFW/glfw3.h>
#include "imGUI.h"
#include <vector>
#include "EditorWidget.h"
#include "Application.h"

class UISetting
{
public:
    ImVec4 clear_color;
};

class EditorGUISystem : public ISubsystem
{
private:
    UISetting Setting;
    std::vector<EditorWidget *> MyWidgets;
    GLFWwindow *window;
    bool isMinimized = false;

private: // ISubsystem Interface
    virtual void Initialize() override;
    virtual void Update() override
    {
        if (canRender())
        {
            Render();
        }
        else
        {
            Application::quit();
        }
    }
    virtual void Shutdown() override;

public:
    const UISetting *GetSetting() const { return &Setting; }
    template <typename T>
    T *CreateWidget();

protected:
    std::vector<EditorWidget *> &GetWidgets() { return MyWidgets; }

private:
    void Render();
    bool canRender() const;
    static void glfw_error_callback(int error, const char *description);
};

template <typename T>
T *EditorGUISystem::CreateWidget()
{
    static_assert(std::is_base_of<EditorWidget, T>::value, "T must derive from Widget");
    T *widgetPtr = new T(this);
    MyWidgets.push_back(widgetPtr);
    return widgetPtr;
}
