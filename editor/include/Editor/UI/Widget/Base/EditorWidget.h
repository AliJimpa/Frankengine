#pragma once

class EditorGUISystem;

// ============================================================
//  EditorWidget - base class for every ImGui panel/window shown
//  in the editor. Create one, register it with
//  EditorGUISystem::CreateWidget<T>(), and it gets drawn every
//  frame inside its own ImGui::Begin()/End() window.
// ============================================================
class EditorWidget
{
public:
    explicit EditorWidget(EditorGUISystem* owner) : Owner(owner) {}
    virtual ~EditorWidget() = default;

    virtual void OnDraw() = 0;
    virtual const char* GetName() const = 0;

protected:
    EditorGUISystem* Owner;
};
