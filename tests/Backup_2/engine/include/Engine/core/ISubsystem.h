#pragma once

// ============================================================
//  ISubsystem - anything registered and driven by the main loop
//  (e.g. EditorGUISystem, a future RenderSystem, AudioSystem...)
// ============================================================
class ISubsystem
{
public:
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Shutdown() = 0;
    virtual ~ISubsystem() = default;
};
