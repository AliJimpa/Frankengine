#pragma once

#ifdef FRANKENGINE_EXPORTS
#define FRANKENGINE_API __declspec(dllexport)
#else
#define FRANKENGINE_API __declspec(dllimport)
#endif

class Logger
{
public:
    virtual void log(const char *msg) = 0;
};

struct EngineAPI
{
    Logger *logger;
};

extern "C" FRANKENGINE_API EngineAPI* GetEngineAPI();