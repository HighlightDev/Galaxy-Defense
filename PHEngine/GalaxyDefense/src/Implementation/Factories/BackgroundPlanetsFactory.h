#pragma once

#include "ISpaceObjectFactory.h"

namespace EngineCore {
class Scene;
}

namespace Game {
class SpaceshipActor;

class BackgroundPlanetsFactory : public IBackgroundSpaceObjectFactory {
    static size_t s_backgroundPlanetCounter;

public:
    BackgroundPlanetsFactory() = default;

    virtual std::shared_ptr<BackgroundSpaceObjectActor> CreateSpaceObject(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const std::string& imageName,
        const float billboardSize) override;
};
} // namespace Game