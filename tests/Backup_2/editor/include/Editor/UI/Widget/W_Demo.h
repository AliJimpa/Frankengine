#pragma once
#include "Editor/UI/Widget/Base/EditorWidget.h"
#include <imgui.h>

class W_Demo : public EditorWidget
{
public:
    using EditorWidget::EditorWidget;

    const char* GetName() const override { return "Demo"; }

    void OnDraw() override
    {
        ImGui::Checkbox("Enabled", &enabled);
        ImGui::SliderFloat("Value", &value, 0.0f, 1.0f);
    }

private:
    bool enabled = true;
    float value = 0.5f;
};
