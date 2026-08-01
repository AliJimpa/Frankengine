#pragma once

// ============================================================
//  EngineAPI - services the Engine exposes to the Game.
//
//  Engine is now a STATIC LIB linked directly into Runtime.exe,
//  so there is no dllexport/dllimport boundary here anymore.
//  Game.dll never links the Engine lib; it only ever sees this
//  struct through the pointer passed into IGame::start().
// ============================================================

class Logger
{
public:
    virtual ~Logger() = default;
    virtual void log(const char* msg) = 0;
};

struct EngineAPI
{
    Logger* logger = nullptr;
};

// Implemented once inside the Engine static lib (src/engine/EngineAPI.cpp).
// Called by Runtime.exe only - Game.dll receives the pointer, it never
// calls this itself.
extern "C" EngineAPI* GetEngineAPI();
