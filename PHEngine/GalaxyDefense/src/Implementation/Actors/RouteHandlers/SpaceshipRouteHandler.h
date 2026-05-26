#pragma once

#include <glm/vec3.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace Game {
class SpaceshipActor;
class OnRouteMovementComponent;

enum class eSpaceshipRouteGoal { NONE, DESTINATION, BARRIER };

std::string ToString(const eSpaceshipRouteGoal goal);

class SpaceshipRouteHandler {
    SpaceshipActor* mSpaceship{nullptr};
    eSpaceshipRouteGoal mCurrentGoal{eSpaceshipRouteGoal::NONE};

    std::shared_ptr<OnRouteMovementComponent> GetMovementComponent() const;

protected:
    std::function<std::vector<glm::vec3>(const glm::vec3&)> mBuildRouteToDestinationFn;
    std::function<std::vector<glm::vec3>(const glm::vec3&)> mBuildRouteToBarrierFn;

    virtual void AssignRoute(const glm::vec3& startPosition) = 0;

    void ApplyRoute(const std::vector<glm::vec3>& routePoints, eSpaceshipRouteGoal routeGoal);

    void StopMovement();

public:
    explicit SpaceshipRouteHandler(SpaceshipActor* spaceship);

    virtual ~SpaceshipRouteHandler() = default;

    void SetRouteBuildFunctions(
        std::function<std::vector<glm::vec3>(const glm::vec3&)> buildRouteToDestinationFn,
        std::function<std::vector<glm::vec3>(const glm::vec3&)> buildRouteToBarrierFn);

    void InitializeAndStartFrom(const glm::vec3& startPosition);

    bool UpdateRoutesAndCheckIfCompleted();

    void RebuildFromCurrentPosition();

    std::vector<glm::vec3> GetCurrentRoutePoints() const;

    void ResetState();
};
} // namespace Game
