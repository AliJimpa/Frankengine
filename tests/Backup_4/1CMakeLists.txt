cmake_minimum_required(VERSION 3.15)
project(CppProjectManager CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(CppProjectManager
    src/main.cpp
    src/ProjectManager.cpp
    src/Registry.cpp
    src/TemplateFiles.cpp
)

target_include_directories(CppProjectManager PRIVATE src)

if (UNIX)
    target_link_libraries(CppProjectManager PRIVATE stdc++fs)
endif()
