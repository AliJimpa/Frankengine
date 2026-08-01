#pragma once

class Application
{
public:
    static void quit() { getInstance().running = false; }
    static bool isRunning() { return getInstance().running; }

private:
    // Private constructor/copy ensures no one else can create an instance
    Application() : running(true) {}
    bool running;
    // This provides global access to the instance
    static Application &getInstance()
    {
        static Application instance; // Guaranteed to be destroyed, instantiated on first use
        return instance;
    }
};
