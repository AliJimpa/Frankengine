#pragma once

class ISubsystem
{
public:
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Shutdown() = 0;
    virtual ~ISubsystem() = default;
};