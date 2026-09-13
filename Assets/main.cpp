#include <iostream>
#include "Library/OpenGl/glfw3.h"

#define LOG(x) std::cout << x << std::endl

int main()
{
    LOG("Start");
    int a = glfwInit();
    LOG("End");

    std::cin.get();
}
