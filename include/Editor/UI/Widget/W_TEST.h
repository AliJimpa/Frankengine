#pragma once

#include "iostream"
#include "Application.h"
#include "EditorWidget.h"

class W_TEST : public EditorWidget
{
public:
    int Numbe = 0;
    W_TEST(EditorGUISystem *owner) : EditorWidget(owner) {
        std::cout << "StartLauncher\n";
    }

    void Draw() override
    {
        // std::cout << "Rendering Button Widget\n";
        ImGui::Begin("Launcher"); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        if (ImGui::TreeNode("Eliding, Alignment"))
        {
            //IMGUI_DEMO_MARKER("Widgets/Text Input/Eliding, Alignment");
            static char buf1[128] = "/path/to/some/folder/with/long/filename.cpp";
            static ImGuiInputTextFlags flags = ImGuiInputTextFlags_ElideLeft;
            ImGui::CheckboxFlags("ImGuiInputTextFlags_ElideLeft", &flags, ImGuiInputTextFlags_ElideLeft);
            ImGui::InputText("Path", buf1, IM_COUNTOF(buf1), flags);
            ImGui::TreePop();
        }
        ImGui::Text("Hello333");
        if (ImGui::Button("QuitApp")) // Buttons return true when clicked (most widgets return true when edited/activated)
            Application::quit();
        ImGui::Text("MyTESTNumberr = %d", Numbe);
        if (ImGui::Button("Add"))
            Numbe++;
        ImGui::End();
    }
};
