#include "Editor/system/EditorGUISystem.h"

#include <windows.h> // must come before GL/gl.h for APIENTRY etc.
#include <GL/gl.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdio>

void EditorGUISystem::glfw_error_callback(int error, const char* description)
{
    std::fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void EditorGUISystem::Initialize()
{
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
        return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1280, 720, "FrankEngine Editor", nullptr, nullptr);
    if (!window)
        return;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

bool EditorGUISystem::canRender() const
{
    return window != nullptr && !glfwWindowShouldClose(window);
}

void EditorGUISystem::Render()
{
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (EditorWidget* widget : MyWidgets)
    {
        if (ImGui::Begin(widget->GetName()))
            widget->OnDraw();
        ImGui::End();
    }

    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(Setting.clear_color.x, Setting.clear_color.y, Setting.clear_color.z, Setting.clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

void EditorGUISystem::Shutdown()
{
    for (EditorWidget* widget : MyWidgets)
        delete widget;
    MyWidgets.clear();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}
