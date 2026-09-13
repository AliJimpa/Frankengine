#pragma once

#include "EditorWidget.h"

class W_Demo : public EditorWidget
{
public:
    bool IsOpen = false;
    W_Demo(EditorGUISystem *owner) : EditorWidget(owner) {}

    void Draw() override
    {
        if (IsOpen)
            ImGui::ShowDemoWindow(&IsOpen);

        ImGui::Begin("DemoTry");
        ImGui::Checkbox("ShowDemoWindow", &IsOpen);
        ImGui::End();
    }
};
