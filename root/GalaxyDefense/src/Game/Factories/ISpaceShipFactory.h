#pragma once

#include <glm/vec3.hpp>

#include <memory>

namespace EngineCore {
class Scene;
}

namespace Game {
class SpaceshipActor;

class ISpaceShipFactory {
public:
    virtual std::shared_ptr<SpaceshipActor> CreateSpaceShip(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const int32_t textFontSize)
        = 0;
};
} // namespace Game