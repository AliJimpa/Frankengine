#include <iostream>
#include "EditorUI.h"
#include "W_Demo.h"
// #include "Library/OpenGL/glfw3.h"

#define LOG(x) std::cout << x << std::endl

bool isRunning = true;

int main()
{
    Initialize();

    W_Demo *widgetPtr = new W_Demo();
    MyWidgets.push_back(widgetPtr);

    while (isRunning)
    {
        if (canRender())
            Render();
    }

    LOG("TEST");

    // int a = glfwInit();
    // LOG(a);

    std::cin.get();
}
