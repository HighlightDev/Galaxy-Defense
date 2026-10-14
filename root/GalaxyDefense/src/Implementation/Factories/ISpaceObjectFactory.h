#pragma once

#include "Implementation/LootCategoryType.h"

#include <glm/vec3.hpp>

#include <memory>
#include <string>

namespace EngineCore {
class Scene;
class Actor;
} // namespace EngineCore

namespace Game {
class BackgroundSpaceObjectActor;
class SpaceObjectActor;
class BarrierActor;
class PortalActor;
class LootActor;

class IBackgroundSpaceObjectFactory {
    virtual std::shared_ptr<BackgroundSpaceObjectActor> CreateSpaceObject(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const std::string& imageName,
        const float billboardSize)
        = 0;
};

class IAsteroidFactory {
    virtual std::shared_ptr<SpaceObjectActor> CreateSpaceObject(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale)
        = 0;
};

class IBarrierFactory {
    virtual std::shared_ptr<BarrierActor> CreateBarrier(
        const int32_t pillarsMeshCount,
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale)
        = 0;
};

class IPortalFactory {
    virtual std::shared_ptr<PortalActor> CreatePortal(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const float billboardSize)
        = 0;
};

class ILootFactory {
    virtual std::shared_ptr<LootActor> CreateLoot(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const eLootCategory lootCategory)
        = 0;
};
} // namespace Game