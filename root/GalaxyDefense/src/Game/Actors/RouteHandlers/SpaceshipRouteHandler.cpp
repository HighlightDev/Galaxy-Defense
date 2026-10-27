#include "SpaceshipRouteHandler.h"

#include "Core/CommonCore/Assertion.h"
#include "Game/Actors/SpaceshipActor.h"
#include "Game/Components/MovementComponents/OnRouteMovementComponent.h"

namespace Game {

std::string ToString(const eSpaceshipRouteGoal goal)
{
    switch (goal) {
    case eSpaceshipRouteGoal::NONE:
        return "NONE";
    case eSpaceshipRouteGoal::DESTINATION:
        return "DESTINATION";
    case eSpaceshipRouteGoal::BARRIER:
        return "BARRIER";
    default:
        return "UNKNOWN";
    }
}

SpaceshipRouteHandler::SpaceshipRouteHandler(std::weak_ptr<SpaceshipActor> spaceship)
    : mSpaceship(spaceship)
{
    ext_assert(!mSpaceship.expired(), "SpaceshipRouteHandler: null spaceship pointer");
}

void SpaceshipRouteHandler::SetRouteBuildFunctions(
    std::function<std::vector<glm::vec3>(const glm::vec3&)> buildRouteToDestinationFn,
    std::function<std::vector<glm::vec3>(const glm::vec3&)> buildRouteToBarrierFn)
{
    mBuildRouteToDestinationFn = std::move(buildRouteToDestinationFn);
    mBuildRouteToBarrierFn = std::move(buildRouteToBarrierFn);
}

void SpaceshipRouteHandler::InitializeAndStartFrom(const glm::vec3& startPosition)
{
    const auto spaceship = mSpaceship.lock();
    ext_assert(spaceship, "SpaceshipRouteHandler spaceship is null in InitializeAndStartFrom");
    spaceship->TriggerSpawn(startPosition);
    AssignRoute(startPosition);
}

bool SpaceshipRouteHandler::UpdateRoutesAndCheckIfCompleted()
{
    const auto spaceship = mSpaceship.lock();
    if (!spaceship || eSpaceshipActivityState::ACTIVE != spaceship->GetSpaceshipActivityState()) {
        return false;
    }

    if (const auto& movementComp = GetMovementComponent()) {
        if (!movementComp->GetIsDistanceCompleted()) {
            return false;
        }

        // Reached a barrier waypoint: try to find a fresh route (destination cell may now be
        // reachable, or a different barrier is closer). Without this, the ship sits forever
        // at the waypoint until some external barrier add/remove triggers a global rebuild.
        if (eSpaceshipRouteGoal::BARRIER == mCurrentGoal) {
            RebuildFromCurrentPosition();
            return false;
        }
    }

    if (eSpaceshipRouteGoal::DESTINATION == mCurrentGoal) {
        ResetState();
        return true;
    }

    return false;
}

void SpaceshipRouteHandler::RebuildFromCurrentPosition()
{
    const auto spaceship = mSpaceship.lock();
    if (!spaceship || eSpaceshipActivityState::ACTIVE != spaceship->GetSpaceshipActivityState()) {
        return;
    }

    AssignRoute(spaceship->GetWorldPosition());
}

std::vector<glm::vec3> SpaceshipRouteHandler::GetCurrentRoutePoints() const
{
    const auto movementComp = GetMovementComponent();
    return movementComp ? movementComp->GetRoutePoints() : std::vector<glm::vec3>{};
}

std::shared_ptr<OnRouteMovementComponent> SpaceshipRouteHandler::GetMovementComponent() const
{
    const auto spaceship = mSpaceship.lock();
    return spaceship ? spaceship->GetOnRouteMovementComponent() : nullptr;
}

int32_t SpaceshipRouteHandler::GetSpaceshipId() const
{
    const auto spaceship = mSpaceship.lock();
    return spaceship ? spaceship->GetObjectId() : -1;
}

void SpaceshipRouteHandler::ApplyRoute(const std::vector<glm::vec3>& routePoints, const eSpaceshipRouteGoal routeGoal)
{
    LogInfo(
        "SpaceshipRouteHandler::ApplyRoute: applying route for spaceship ",
        GetSpaceshipId(),
        " with goal ",
        ToString(routeGoal),
        " and points count: ",
        routePoints.size());
    const auto movementComp = GetMovementComponent();
    ext_assert(movementComp, "SpaceshipRouteHandler movement component is null in ApplyRoute");

    movementComp->ResetStates();
    movementComp->SetRoutePoints(routePoints);

    movementComp->SetIsMovementOnRouteAllowed(true);
    mCurrentGoal = routeGoal;
}

void SpaceshipRouteHandler::StopMovement()
{
    LogInfo(
        "SpaceshipRouteHandler::StopMovement: stopping movement for spaceship ",
        GetSpaceshipId(),
        ", was goal: ",
        ToString(mCurrentGoal));
    const auto movementComp = GetMovementComponent();
    if (movementComp) {
        movementComp->ResetStates();
    }
}

void SpaceshipRouteHandler::ResetState()
{
    LogInfo(
        "SpaceshipRouteHandler::ResetState: resetting route handler state for spaceship ",
        GetSpaceshipId(),
        ", was goal: ",
        ToString(mCurrentGoal));
    StopMovement();
    mCurrentGoal = eSpaceshipRouteGoal::NONE;
}

} // namespace Game
