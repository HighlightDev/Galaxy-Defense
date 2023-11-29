#pragma once

#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"

#include <vector>
#include <glm/vec3.hpp>

using namespace EngineCore;

namespace Game
{
    class OnRouteMovementComponent
        : public NoPhysicsMovementComponent
    {
    protected:
        std::vector<glm::vec3> mRoutePoints;

        // param which lies between 0 and 1 and denotes route movement completion
        float mCurrentDistanceOnRoute{0.0f};

        bool mIsMovementAllowed{false};

        float mRouteTotalDistance{0.0f};

    public:
        explicit OnRouteMovementComponent(const std::shared_ptr<MovementComponentData> &movementComponentData);

        void Move(const float deltaTime) override;

        void SetRoutePoints(const std::vector<glm::vec3> &routePoints);

        const std::vector<glm::vec3> &GetRoutePoints() const;

        void TeleportToDistanceOnRoute(const float distance);

        float GetCurrentDistanceOnRoute() const;

        bool GetIsMovementAllowed() const;

        void SetIsMovementAllowed(const bool isAllowed);

    private:
        void CalculateRouteTotalDistance();
    };
}
