#pragma once
#include <chrono>

class DeltaTimer
{
    std::chrono::steady_clock::time_point lastTime;

public:
    DeltaTimer() : lastTime(std::chrono::steady_clock::now()) {}

    float getDeltaTime()
    {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        lastTime = currentTime;
        return elapsed.count();
    }
};
