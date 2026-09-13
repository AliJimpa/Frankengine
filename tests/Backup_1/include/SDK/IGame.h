#pragma once
#include "EngineAPI.h"

class IGame
{
public:
    virtual void start(EngineAPI* api) = 0;
    virtual void update() = 0;
};

typedef IGame* (*CreateGameFunc)();
