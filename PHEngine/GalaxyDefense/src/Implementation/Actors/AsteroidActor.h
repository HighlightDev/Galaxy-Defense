#pragma once

#include "SpaceObjectActor.h"

#include <functional>
#include <memory>
#include <unordered_map>

using namespace EngineCore;

namespace Game {
class AsteroidActor : public SpaceObjectActor {
public:
    AsteroidActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);
};
} // namespace Game