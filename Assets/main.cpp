#include <iostream>
#include "Library/OpenGL_Static_x86/glfw3.h"

#define LOG(x) std::cout << x << std::endl

int main()
{
    LOG("Start");
    int a = glfwInit();
    LOG("End");

    std::cin.get();
}
