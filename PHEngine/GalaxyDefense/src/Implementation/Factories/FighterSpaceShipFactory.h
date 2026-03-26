#pragma once

#include "ISpaceShipFactory.h"

namespace EngineCore {
class Scene;
}

namespace Graphics {
class IMaterial;
}

namespace Game {
class SpaceshipActor;

class FighterSpaceShipFactory : public ISpaceShipFactory {
    static size_t s_fighterSpaceShipCounter;

public:
    FighterSpaceShipFactory() = default;

    virtual std::shared_ptr<SpaceshipActor> CreateSpaceShip(
        const std::shared_ptr<::EngineCore::Scene>& scene,
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const int32_t textFontSize) override;

    std::shared_ptr<::Graphics::IMaterial>
    GetMaterial(const std::shared_ptr<::EngineCore::Scene>& scene, bool& alreadyExists) const;
};
} // namespace Game
