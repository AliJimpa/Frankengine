#pragma once

#include "EditorGUISystem.h"
#include "App.h"

class UserInterface : public EditorGUISystem
{
public:
    UserInterface(App *owner);
};