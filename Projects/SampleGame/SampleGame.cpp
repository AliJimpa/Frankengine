#include "SDK/IGame.h"

// ============================================================
//  SampleGame - minimal Game.dll, compiled separately from the
//  Engine/Editor/Runtime. Only depends on the SDK headers.
// ============================================================
class SampleGame : public IGame
{
public:
    void start(EngineAPI* api) override
    {
        m_api = api;
        if (m_api && m_api->logger)
            m_api->logger->log("SampleGame started!");
    }

    void update() override
    {
        if (m_api && m_api->logger)
            m_api->logger->log("SampleGame update tick");
    }

    void shutdown() override
    {
        if (m_api && m_api->logger)
            m_api->logger->log("SampleGame shutting down");
    }

private:
    EngineAPI* m_api = nullptr;
};

extern "C" __declspec(dllexport) IGame* createGame()
{
    return new SampleGame();
}
