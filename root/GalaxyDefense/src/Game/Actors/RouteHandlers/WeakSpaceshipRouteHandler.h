#pragma once

#include "SpaceshipRouteHandler.h"

namespace Game {

class WeakSpaceshipRouteHandler : public SpaceshipRouteHandler {
public:
    explicit WeakSpaceshipRouteHandler(SpaceshipActor* spaceship);

protected:
    void AssignRoute(const glm::vec3& startPosition) override;
};

} // namespace Game
