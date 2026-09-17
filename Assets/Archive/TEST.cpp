#include <iostream>
#include "Library/OpenGL/glfw3.h"

#define LOG(x) std::cout << x << std::endl

int main()
{
    int a = glfwInit();
    LOG(a);

    std::cin.get();
}
