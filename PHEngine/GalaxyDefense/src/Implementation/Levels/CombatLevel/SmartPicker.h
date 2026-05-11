#pragma once

#include "Implementation/GameObjectsType.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <unordered_set>

namespace EngineCore {
class Scene;
class ACamera;
} // namespace EngineCore

namespace Game {
class CombatActorsPoolHandler;

class SmartPicker {
    std::shared_ptr<CombatActorsPoolHandler> mCombatActorsPoolHandler;

public:
    explicit SmartPicker(const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler);

    glm::vec3 CreateWorldSpaceRayFromScreenSpacePosition(
        const std::shared_ptr<::EngineCore::ACamera>& camera, const glm::ivec2& screenSpacePosition) const;

    int32_t CastScreenSpaceRayIntoScene(
        const std::shared_ptr<::EngineCore::Scene>& sceneSp,
        const std::shared_ptr<::EngineCore::ACamera>& camera,
        const glm::ivec2& screenSpacePosition,
        const std::unordered_set<eGameObjectsType>& gameObjectTypesToIgnore = {}) const;
};
} // namespace Game
