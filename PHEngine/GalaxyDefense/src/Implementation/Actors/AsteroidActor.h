#pragma once

#include "SpaceObjectActor.h"

#include <functional>
#include <unordered_map>
#include <memory>

using namespace EngineCore;

namespace Game
{
    class AsteroidActor
        : public SpaceObjectActor
    {
    public:
        AsteroidActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent);
    };
}