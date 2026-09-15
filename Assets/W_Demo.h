#pragma once

class W_Demo : public IDraw
{
public:
    bool IsOpen = false;

    void Draw() override
    {
        if (IsOpen)
            ImGui::ShowDemoWindow(&IsOpen);

        ImGui::Begin("DemoTry");
        ImGui::Checkbox("ShowDemoWindow", &IsOpen);
        ImGui::End();
    }
};
