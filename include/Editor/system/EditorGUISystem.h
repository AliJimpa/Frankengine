#pragma once

#include "Engine/core/ISubsystem.h"
#include "Engine/core/Application.h"
#include "Editor/UI/Widget/Base/EditorWidget.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <vector>
#include <type_traits>

class UISetting
{
public:
    ImVec4 clear_color = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
};

// ============================================================
//  EditorGUISystem - owns the GLFW window, the ImGui context,
//  and every EditorWidget drawn each frame.
//
//  NOTE: Initialize/Update/Shutdown are public here (unlike the
//  first draft) so App can call them directly through an
//  EditorGUISystem* without going through ISubsystem*.
// ============================================================
class EditorGUISystem : public ISubsystem
{
public:
    void Initialize() override;
    void Update() override
    {
        if (canRender())
            Render();
        else
            Application::quit();
    }
    void Shutdown() override;

public:
    const UISetting* GetSetting() const { return &Setting; }

    template <typename T>
    T* CreateWidget()
    {
        static_assert(std::is_base_of<EditorWidget, T>::value, "T must derive from EditorWidget");
        T* widgetPtr = new T(this);
        MyWidgets.push_back(widgetPtr);
        return widgetPtr;
    }

protected:
    std::vector<EditorWidget*>& GetWidgets() { return MyWidgets; }

private:
    void Render();
    bool canRender() const;
    static void glfw_error_callback(int error, const char* description);

private:
    UISetting Setting;
    std::vector<EditorWidget*> MyWidgets;
    GLFWwindow* window = nullptr;
};
