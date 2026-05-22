#include "IGame.h"

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

extern "C" FRANKENGINE_API IGame* createGame()
{
    return new Game();
}