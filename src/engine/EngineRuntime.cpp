#include "SDK/EngineAPI.h"
#include <iostream>

class LoggerImpl : public Logger
{
public:
    void log(const char* msg) override
    {
        std::cout << "[Engine] " << msg << std::endl;
    }
};

static LoggerImpl logger;
static EngineAPI api;

extern "C" FRANKENGINE_API EngineAPI* GetEngineAPI()
{
    api.logger = &logger;
    return &api;
}
