#pragma once
#include "EngineAPI.h"

// ============================================================
//  IGameModule - implemented by any Game.dll (a "module"/plugin),
//  driven by Runtime.exe.
//
//  This header is shared by EVERY project and must stay neutral -
//  it never contains a project-specific export macro. Each project
//  defines its OWN <ProjectName>_API macro in its own small header
//  (see projects/SampleGame/Source/SampleGameAPI.h), exactly like
//  an Unreal plugin's generated <MODULE>_API - so multiple modules
//  never collide or share export state.
//
//  A module must:
//    1) implement a class derived from IGameModule
//       (marked with its own <ProjectName>_API macro)
//    2) export: extern "C" <ProjectName>_API IGameModule* createModule();
//
//  Runtime.exe scans its own folder for *.dll at startup, and
//  loads every one that exports createModule() as a module - it
//  finds it purely by that literal string via GetProcAddress, so
//  it never needs to know any project's API macro name.
// ============================================================

class IGameModule
{
public:
    virtual ~IGameModule() = default;

    virtual void start(EngineAPI* api) = 0;
    virtual void update() = 0;
    virtual void shutdown() {}
};

typedef IGameModule* (*CreateModuleFunc)();