#pragma once
#include "IDraw.h"

class EditorGUISystem;
class UISetting;

class EditorWidget : public IDraw
{
private:
    EditorGUISystem *MyOwner;

public:
    EditorWidget(EditorGUISystem *owner) : MyOwner(owner) {};

public: // IDraw Interface
    virtual void Draw() = 0;

protected:
    const EditorGUISystem *GetOwner() const { return MyOwner; }
};