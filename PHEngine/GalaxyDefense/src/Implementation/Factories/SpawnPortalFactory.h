#pragma once

#include "ISpaceObjectFactory.h"

namespace EngineCore {
class Scene;
class Actor;
} // namespace EngineCore

namespace Game {
class SpawnPortalFactory : public IPortalFactory {
    static size_t s_portalCounter;

public:
    SpawnPortalFactory() = default;

    virtual std::shared_ptr<::EngineCore::Actor> CreatePortal(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const float billboardSize) override;
};
} // namespace Game