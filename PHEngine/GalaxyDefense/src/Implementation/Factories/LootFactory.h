#pragma once

#include "ISpaceObjectFactory.h"

namespace EngineCore {
class Scene;
} // namespace EngineCore

namespace Game {

class LootActor;

class LootFactory : public ILootFactory {
    static size_t s_lootCounter;

public:
    LootFactory() = default;

    virtual std::shared_ptr<LootActor> CreateLoot(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const eLootCategory lootCategory) override;
};
} // namespace Game