#include "SpaceshipRouteHandler.h"

#include "Core/CommonCore/Assertion.h"
#include "Implementation/Actors/SpaceshipActor.h"
#include "Implementation/Components/MovementComponents/OnRouteMovementComponent.h"

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

SpaceshipRouteHandler::SpaceshipRouteHandler(SpaceshipActor* spaceship)
    : mSpaceship(spaceship)
{
    ext_assert(mSpaceship, "SpaceshipRouteHandler: null spaceship pointer");
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
    ext_assert(mSpaceship, "SpaceshipRouteHandler spaceship is null in InitializeAndStartFrom");
    mSpaceship->TriggerSpawn(startPosition);
    AssignRoute(startPosition);
}

bool SpaceshipRouteHandler::UpdateRoutesAndCheckIfCompleted()
{
    if (!mSpaceship || eSpaceshipActivityState::ACTIVE != mSpaceship->GetSpaceshipActivityState()) {
        return false;
    }

    if (const auto& movementComp = GetMovementComponent()) {
        if (!movementComp->GetIsDistanceCompleted()) {
            return false;
        }

        if (eSpaceshipRouteGoal::BARRIER == mCurrentGoal && movementComp->GetIsDistanceCompleted()) {
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
    if (!mSpaceship || eSpaceshipActivityState::ACTIVE != mSpaceship->GetSpaceshipActivityState()) {
        return;
    }

    AssignRoute(mSpaceship->GetWorldPosition());
}

std::vector<glm::vec3> SpaceshipRouteHandler::GetCurrentRoutePoints() const
{
    const auto movementComp = GetMovementComponent();
    return movementComp ? movementComp->GetRoutePoints() : std::vector<glm::vec3>{};
}

std::shared_ptr<OnRouteMovementComponent> SpaceshipRouteHandler::GetMovementComponent() const
{
    return mSpaceship ? mSpaceship->GetOnRouteMovementComponent() : nullptr;
}

void SpaceshipRouteHandler::ApplyRoute(const std::vector<glm::vec3>& routePoints, const eSpaceshipRouteGoal routeGoal)
{
    LogInfo(
        "SpaceshipRouteHandler::ApplyRoute: applying route for spaceship ",
        mSpaceship->GetObjectId(),
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
        mSpaceship->GetObjectId(),
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
        mSpaceship->GetObjectId(),
        ", was goal: ",
        ToString(mCurrentGoal));
    StopMovement();
    mCurrentGoal = eSpaceshipRouteGoal::NONE;
}

} // namespace Game
