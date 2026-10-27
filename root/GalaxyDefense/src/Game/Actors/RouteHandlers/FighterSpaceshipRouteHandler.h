#pragma once

#include "SpaceshipRouteHandler.h"

namespace Game {

class FighterSpaceshipRouteHandler : public SpaceshipRouteHandler {
public:
    explicit FighterSpaceshipRouteHandler(std::weak_ptr<SpaceshipActor> spaceship);

protected:
    void AssignRoute(const glm::vec3& startPosition) override;
};

} // namespace Game
