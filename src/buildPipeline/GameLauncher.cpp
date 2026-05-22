#include <windows.h>
#include <iostream>
#include "SDK/IGame.h"

int main()
{
    HMODULE engine = LoadLibrary("EngineRuntime.dll");
    if (!engine)
    {
        std::cout << "Engine not found\n";
        return 1;
    }

    auto GetAPI = (EngineAPI*(*)())GetProcAddress(engine, "GetEngineAPI");
    EngineAPI* api = GetAPI();

    HMODULE game = LoadLibrary("Game.dll");
    if (!game)
    {
        std::cout << "Game.dll not found\n";
        return 1;
    }

    auto create = (CreateGameFunc)GetProcAddress(game, "createGame");
    IGame* g = create();

    g->start(api);

    while (true)
    {
        g->update();
        Sleep(1000);
    }

    return 0;
}
