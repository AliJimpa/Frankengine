#define SAMPLEGAME_EXPORTS
#include "SampleGameAPI.h"
#include "IGameModule.h"

// ============================================================
//  SampleGame - a minimal module (Game.dll). The class itself is
//  marked SAMPLEGAME_API (its own project-owned macro), same as
//  how an Unreal plugin marks its public classes with its
//  generated <MODULE>_API - not a shared/generic one.
// ============================================================
class SAMPLEGAME_API SampleGame : public IGameModule
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
        // if (m_api && m_api->logger)
        //     m_api->logger->log("SampleGame update tick");
    }

    void shutdown() override
    {
        if (m_api && m_api->logger)
            m_api->logger->log("SampleGame shutting down");
    }

private:
    EngineAPI* m_api = nullptr;
};

extern "C" SAMPLEGAME_API IGameModule* createModule()
{
    return new SampleGame();
}