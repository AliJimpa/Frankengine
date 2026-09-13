#pragma once

#include "iostream"
// #include "Application.h"
#include "EditorWidget.h"

class W_TEST : public EditorWidget
{
private:
    int Numbe = 0;
    bool IsOpen = true;
    ImGuiWindowFlags window_flags = 0;
    char buf1[500] = "/path/to/some/folder/with/long/filename.cpp";
    ImGuiInputTextFlags InputTextflag = ImGuiInputTextFlags_ElideLeft;

public:
    W_TEST(EditorGUISystem *owner) : EditorWidget(owner)
    {
        std::cout << "StartLauncher\n";
    }

    void Draw() override
    {
        // const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
        // ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
        // ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

        if (!ImGui::Begin("L", &IsOpen, window_flags))
        {
            ImGui::End();
            return;
        }

        if (ImGui::CollapsingHeader("CreateProject"))
        {
            // ImGui::CheckboxFlags("ImGuiInputTextFlags_ElideLeft", &InputTextflag, ImGuiInputTextFlags_ElideLeft);
            ImGui::InputText("Path", buf1, IM_COUNTOF(buf1), InputTextflag);

            // IMGUI_DEMO_MARKER("Help");
            // ImGui::SeparatorText("ABOUT THIS DEMO:");
            // ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
            // ImGui::BulletText("The \"Examples\" menu above leads to more demo contents.");
            // ImGui::BulletText("The \"Tools\" menu above gives access to: About Box, Style Editor,\n"
            //                   "and Metrics/Debugger (general purpose Dear ImGui debugging tool).");
            // ImGui::BulletText("Web demo (w/ source code browser): ");
            // ImGui::SameLine(0, 0);
            // ImGui::TextLinkOpenURL("https://pthom.github.io/imgui_explorer");
        }

        // ImGui::PopItemWidth();
        ImGui::End();

        // std::cout << "Rendering Button Widget\n";
        // ImGui::Begin("Launcher"); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        // if (ImGui::TreeNode("Eliding, Alignment"))
        // {
        //     // IMGUI_DEMO_MARKER("Widgets/Text Input/Eliding, Alignment");
        // ImGui::TreePop();
        // }
        // ImGui::Text("Hello333");
        // if (ImGui::Button("QuitApp")) // Buttons return true when clicked (most widgets return true when edited/activated)
        //     Application::quit();
        // ImGui::Text("MyTESTNumberr = %d", Numbe);
        // if (ImGui::Button("Add"))
        //     Numbe++;
        // ImGui::End();
    }
};
