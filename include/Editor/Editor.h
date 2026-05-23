#pragma once

#include "App.h"

class Editor : public App
{
    void OnStart() override {};
    void OnLateUpdate() override {};
    void OnUpdate() override {};
    void OnEnd() override {};
    bool CanLoop() const override { return App::CanLoop(); };
};