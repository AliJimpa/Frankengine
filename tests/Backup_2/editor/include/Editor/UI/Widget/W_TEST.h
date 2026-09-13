#pragma once
#include "Editor/UI/Widget/Base/EditorWidget.h"
#include <imgui.h>

class W_TEST : public EditorWidget
{
public:
    using EditorWidget::EditorWidget;

    const char* GetName() const override { return "Test"; }

    void OnDraw() override
    {
        ImGui::Text("Click count: %d", clickCount);
        if (ImGui::Button("Click me"))
            clickCount++;
    }

private:
    int clickCount = 0;
};
