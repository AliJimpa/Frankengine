#include "EngineAPI.h"
#include <cstdio>

namespace
{
    class ConsoleLogger : public Logger
    {
    public:
        void log(const char* msg) override
        {
            std::printf("[Engine] %s\n", msg);
        }
    };

    ConsoleLogger g_ConsoleLogger;
    EngineAPI g_EngineAPI{ &g_ConsoleLogger };
}

extern "C" EngineAPI* GetEngineAPI()
{
    return &g_EngineAPI;
}
