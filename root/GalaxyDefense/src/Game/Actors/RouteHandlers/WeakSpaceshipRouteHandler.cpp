#include "WeakSpaceshipRouteHandler.h"

namespace Game {

WeakSpaceshipRouteHandler::WeakSpaceshipRouteHandler(std::weak_ptr<SpaceshipActor> spaceship)
    : SpaceshipRouteHandler(spaceship)
{
}

void WeakSpaceshipRouteHandler::AssignRoute(const glm::vec3& startPosition)
{
    const auto route = mBuildRouteToDestinationFn(startPosition);
    if (!route.empty()) {
        ApplyRoute(route, eSpaceshipRouteGoal::DESTINATION);
    } else {
        ResetState();
    }
}

} // namespace Game
