#pragma once

#include "Widget.h"

class W_TEST : public Widget
{
public:
    int Numbe = 0;

    W_TEST(Graphic *owner, App *application) : Widget(owner, application) {}
    void OnRender() override
    {
        // std::cout << "Rendering Button Widget\n";
        ImGui::Begin("Arsalan"); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello");
        if (ImGui::Button("QuitApp")) // Buttons return true when clicked (most widgets return true when edited/activated)
            GetApp()->Quit();
        ImGui::Text("MyTESTNumberr = %d", Numbe);
        if (ImGui::Button("Add"))
            Numbe++;
        ImGui::End();
    }
};
