#pragma once
#include "EngineAPI.h"

// ============================================================
//  IGame - implemented by Game.dll, driven by Runtime.exe.
//
//  A project's Game.dll must:
//    1) implement a class derived from IGame
//    2) export a C function: IGame* createGame();
// ============================================================
class IGame
{
public:
    virtual ~IGame() = default;

    virtual void start(EngineAPI* api) = 0;
    virtual void update() = 0;
    virtual void shutdown() {}
};

typedef IGame* (*CreateGameFunc)();
