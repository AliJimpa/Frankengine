#include <iostream>
#include "EditorUI.h"
#include "W_Demo.h"

#define LOG(x) std::cout << x << std::endl

bool isRunning;

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

    std::cin.get();
}
