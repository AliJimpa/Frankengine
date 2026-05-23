#pragma once

#include "Widget.h"

class W_Demo : public Widget
{
public:
    bool IsOpen = false;
    W_Demo(Graphic *owner, App *application) : Widget(owner, application) {}

    void OnRender() override
    {
        if (IsOpen)
            ImGui::ShowDemoWindow(&IsOpen);

        ImGui::Begin("DemoTry");
        ImGui::Checkbox("ShowDemoWindow", &IsOpen);
        ImGui::End();
    }
};
