cmake_minimum_required(VERSION 3.15)

project(Frankengine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

##### EngineRuntime.dll #####

add_library(EngineRuntime SHARED
    src/engine/EngineRuntime.cpp
)
target_compile_definitions(EngineRuntime PRIVATE FRANKENGINE_EXPORTS)
target_include_directories(EngineRuntime
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SDK
)
set_target_properties(EngineRuntime PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>"
)

##### Editor.exe #####

add_executable(Editor
    src/editor/Editor.cpp
)
target_include_directories(Editor
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
# target_link_libraries(Editor
#     PRIVATE
#     EngineRuntime
# )
target_link_options(Editor PRIVATE /INCREMENTAL:NO)
set_target_properties(Editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>"
)



##### BuildPipeline.exe #####

add_executable(GameLauncher
    src/buildPipeline/GameLauncher.cpp
)
target_include_directories(GameLauncher
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SDK
)
set_target_properties(GameLauncher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/BuildPipeline"
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

# Different compiler options for Debug vs Release
# target_compile_options(EngineRuntime PRIVATE
#     $<$<CONFIG:Debug>:-g>   # example for GCC/Clang
#     $<$<CONFIG:Release>:-O3>
# )

# Optional — different compiler flags for each config
# target_compile_options(EngineRuntime PRIVATE
#     $<$<CXX_COMPILER_ID:MSVC>:/MP>
#     $<$<CONFIG:Debug>:/Zi>      # PDB debug info
#     $<$<CONFIG:Release>:/O2>    # Optimization
# )