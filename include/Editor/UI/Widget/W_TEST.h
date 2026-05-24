#pragma once

#include "Engine.h"
#include "EditorWidget.h"

class W_TEST : public EditorWidget
{
public:
    int Numbe = 0;
    W_TEST(EditorGUISystem *owner) : EditorWidget(owner) {}

    void Draw() override
    {
        // std::cout << "Rendering Button Widget\n";
        ImGui::Begin("Arsalan"); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello2222");
        if (ImGui::Button("QuitApp")) // Buttons return true when clicked (most widgets return true when edited/activated)
            Quit();
        ImGui::Text("MyTESTNumberr = %d", Numbe);
        if (ImGui::Button("Add"))
            Numbe++;
        ImGui::End();
    }
};
