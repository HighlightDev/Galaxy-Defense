#pragma once

#include "AiBrain.h"

namespace Game
{
    AiBrain::AiBrain(const std::weak_ptr<Scene> &scene)
        : mScene(scene)
    {
    }

    void AiBrain::PreInit()
    {
    }

    void AiBrain::PostInit()
    {
    }

    void AiBrain::Tick(const float deltaTime)
    {
    }
}