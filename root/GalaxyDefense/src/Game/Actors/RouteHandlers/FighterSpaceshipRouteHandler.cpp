#include "FighterSpaceshipRouteHandler.h"

namespace Game {

FighterSpaceshipRouteHandler::FighterSpaceshipRouteHandler(SpaceshipActor* spaceship)
    : SpaceshipRouteHandler(spaceship)
{
}

void FighterSpaceshipRouteHandler::AssignRoute(const glm::vec3& startPosition)
{
    auto route = mBuildRouteToDestinationFn(startPosition);
    if (!route.empty()) {
        ApplyRoute(route, eSpaceshipRouteGoal::DESTINATION);
        return;
    }

    route = mBuildRouteToBarrierFn(startPosition);
    if (!route.empty()) {
        ApplyRoute(route, eSpaceshipRouteGoal::BARRIER);
    } else {
        ResetState();
    }
}

} // namespace Game
