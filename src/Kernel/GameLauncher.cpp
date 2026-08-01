#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "SDK/IGame.h"

// ============================================================
//  Runtime.exe
//
//  Fixed vs. the first draft:
//   - Engine is now a static lib linked directly into this exe,
//     so GetEngineAPI() is a normal call - no more
//     LoadLibrary("EngineRuntime.dll") for it.
//   - Only Game.dll is still loaded dynamically, since that's
//     the piece that changes per-project / gets rebuilt by the
//     Editor's "Play" button.
//   - Sleep(1000) was almost certainly a placeholder bug
//     (1 update per second); switched to ~60 updates/sec.
// ============================================================
int main()
{
    EngineAPI* api = GetEngineAPI();

    HMODULE game = LoadLibrary("Game.dll");
    if (!game)
    {
        std::cout << "Game.dll not found\n";
        return 1;
    }

    auto create = (CreateGameFunc)GetProcAddress(game, "createGame");
    if (!create)
    {
        std::cout << "createGame() export not found in Game.dll\n";
        return 1;
    }

    IGame* g = create();
    g->start(api);

    while (true)
    {
        g->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    g->shutdown();
    delete g;
    FreeLibrary(game);
    return 0;
}
