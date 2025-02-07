#pragma once

#include "ISpaceStationFactory.h"

namespace EngineCore {
class Scene;
}

namespace Game {
class SpaceStationFactory : public ISpaceStationFactory {
public:
    SpaceStationFactory() = default;

    virtual std::shared_ptr<SpaceStationActor> CreateSpaceStation(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const std::string& towerName,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale) const override;
};
} // namespace Game
