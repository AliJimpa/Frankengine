#include "EditorGUISystem.h"
#include <iostream>
#include <stdio.h>
#include "backends/imGUI_impl_glfw.h"
#include "backends/imGUI_impl_opengl2.h"

void EditorGUISystem::Initialize()
{
    // SetDebugFunction pointer
    glfwSetErrorCallback(glfw_error_callback);

    // CheckInitiate Validate
    if (!glfwInit()) 
        return;

    //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // This hides the window from the OS

    // Create window with graphics context
    //GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    //const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    window = glfwCreateWindow(20, 20, "Frankengine", nullptr, nullptr);
    if (window == nullptr)
        return;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::SetCurrentContext(ImGui::GetCurrentContext()); // Set the current context
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    // Basic Parameter
    Setting.clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    return;
}

void EditorGUISystem::Render()
{
    glfwPollEvents();
    isMinimized = (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0);

    if (isMinimized)
    {
        // Still sleep to save CPU, but don't 'return'
        ImGui_ImplGlfw_Sleep(10);
    }
    else
    {
        // Only run UI logic when NOT minimized
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        for (auto &widget : GetWidgets())
        {
            if (widget != nullptr)
                widget->Draw();
        }
        ImGui::Render();
    }

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(Setting.clear_color.x * Setting.clear_color.w, Setting.clear_color.y * Setting.clear_color.w, Setting.clear_color.z * Setting.clear_color.w, Setting.clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}

void EditorGUISystem::Shutdown()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    MyWidgets.clear();
}

bool EditorGUISystem::canRender() const
{
    return !glfwWindowShouldClose(window);
}

void EditorGUISystem::glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}
