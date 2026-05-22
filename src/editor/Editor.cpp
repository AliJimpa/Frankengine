#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void createProject(const std::string& name)
{
    fs::create_directories("Projects/" + name + "/scripts");
    fs::create_directories("Projects/" + name + "/build");

    std::ofstream file("Projects/" + name + "/scripts/Game.cpp");

    file <<
R"(#include "IGame.h"

class Game : public IGame
{
    EngineAPI* api;

public:
    void start(EngineAPI* engine) override
    {
        api = engine;
        api->logger->log("Game started!");
    }

    void update() override
    {
        api->logger->log("Updating...");
    }
};

extern "C" __declspec(dllexport)
IGame* createGame()
{
    return new Game();
})";

    file.close();

    std::cout << "Project created!\n";
}

void buildProject(const std::string& name)
{
    std::string base = "Projects/" + name;

    std::string compileCmd =
        "g++ -shared " + base + "/scripts/Game.cpp "
        "-I SDK -o " + base + "/build/Game.dll";

    system(compileCmd.c_str());

    fs::copy_file("EngineRuntime.dll",
                  base + "/build/EngineRuntime.dll",
                  fs::copy_options::overwrite_existing);

    fs::copy_file("BuildPipeline/GameLauncher.exe",
                  base + "/build/" + name + ".exe",
                  fs::copy_options::overwrite_existing);

    std::cout << "Build complete!\n";
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage:\n";
        std::cout << "Editor new ProjectName\n";
        std::cout << "Editor build ProjectName\n";
        return 0;
    }

    std::string command = argv[1];
    std::string project = argv[2];

    if (command == "new")
        createProject(project);

    if (command == "build")
        buildProject(project);

    return 0;
}
