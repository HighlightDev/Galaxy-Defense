#pragma once

#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"

#include <glm/vec3.hpp>

#include <utility>
#include <vector>

using namespace EngineCore;

namespace Game {
class OnRouteMovementComponent : public NoPhysicsMovementComponent {
protected:
    std::vector<std::pair<float /*total distance from the beginning of the route*/, glm::vec3>> mRoutePoints;

    // param which lies between 0 and 1 and denotes route movement progress
    float mMovementProgressOnRoute{0.0f};

    bool mIsMovementOnRouteAllowed{false};

    float mRouteTotalDistance{0.0f};

    bool mIsDistanceCompleted{false};

public:
    explicit OnRouteMovementComponent(const std::shared_ptr<MovementComponentData>& movementComponentData);

    void Move(const float deltaTime) override;

    void SetRoutePoints(const std::vector<glm::vec3>& routePoints);

    std::vector<glm::vec3> GetRoutePoints() const;

    void TeleportToMovementProgressOnRoute(const float seekDistance);

    float GetMovementProgressOnRoute() const;

    bool GetIsMovementOnRouteAllowed() const;

    void SetIsMovementOnRouteAllowed(const bool isAllowed);

    bool GetIsDistanceCompleted() const;

    void ResetStates();
};
} // namespace Game
