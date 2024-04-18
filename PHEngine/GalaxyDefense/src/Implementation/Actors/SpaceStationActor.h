#pragma once

#include "Core/GameCore/Actor.h"

using namespace EngineCore;

namespace Game
{
    class SpaceStationActor
        : public Actor
    {
    public:
        SpaceStationActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent);
    };
}
