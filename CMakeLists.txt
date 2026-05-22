cmake_minimum_required(VERSION 3.15)

project(Frankengine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

##### EngineRuntime.dll #####

add_library(EngineRuntime SHARED
    src/engine/EngineRuntime.cpp
)
target_include_directories(EngineRuntime
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SDK
)
set_target_properties(EngineRuntime PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>"
)

##### Editor.exe #####

add_executable(Editor
    src/main.cpp
)
target_include_directories(Editor
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
# target_link_libraries(Editor
#     PRIVATE
#     EngineRuntime
# )
set_target_properties(Editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>"
)


##### Global Setting #####
# file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
# )

# list(REMOVE_ITEM ENGINE_SOURCES"${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
# target_include_directories(EngineRuntime PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
#     ${CMAKE_CURRENT_SOURCE_DIR}/include/SDK
# )

# add_executable(Editor src/main.cpp)
# target_include_directories(Editor PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
#     ${CMAKE_CURRENT_SOURCE_DIR}/include/SDK
# )
# target_link_libraries(Editor PRIVATE EngineRuntime)