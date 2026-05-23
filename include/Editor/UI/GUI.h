#pragma once

#include <GLFW/glfw3.h>
#include "imgui.h"
#include <vector>
#include "Widget.h"

class App;

class UISetting
{
public:
    ImVec4 clear_color;
    int width = 640;
    int height = 480;
};

class Graphic
{
public:
    Graphic(App *owner);
    Graphic(App *owner, UISetting &setting);
    bool init();
    void render();
    void shutdown();
    bool canRender() const;
    UISetting *GetSetting() { return &Setting; }

protected:
    App *GetApp() const { return MyOwner; }
    std::vector<Widget *> &GetWidgets() { return MyWidgets; }
    template <typename T>
    T *CreateWidget();

private:
    UISetting Setting;
    App *MyOwner;
    std::vector<Widget *> MyWidgets;
    GLFWwindow *window;
    bool ShowUI = true;
    static void glfw_error_callback(int error, const char *description);
};

template <typename T>
T *Graphic::CreateWidget()
{
    static_assert(std::is_base_of<Widget, T>::value, "T must derive from Widget");
    T *widgetPtr = new T(this, GetApp());
    MyWidgets.push_back(widgetPtr);
    return widgetPtr;
}
