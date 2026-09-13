#pragma once
#include "Editor/UI/Widget/Base/EditorWidget.h"
#include <imgui.h>

class W_HelloWorld : public EditorWidget
{
public:
    using EditorWidget::EditorWidget;

    const char* GetName() const override { return "Hello World"; }

    void OnDraw() override
    {
        ImGui::Text("Hello, FrankEngine!");
        ImGui::Separator();
        ImGui::TextWrapped("This panel proves EditorGUISystem, GLFW and ImGui are wired up correctly.");
    }
};
