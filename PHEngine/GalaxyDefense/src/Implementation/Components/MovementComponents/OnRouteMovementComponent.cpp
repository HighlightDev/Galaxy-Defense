#include "OnRouteMovementComponent.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtx/quaternion.hpp>

using namespace EngineMath;

namespace Game
{
    OnRouteMovementComponent::OnRouteMovementComponent(const std::shared_ptr<MovementComponentData> &movementComponentData)
        : NoPhysicsMovementComponent(movementComponentData)
    {
    }

    void OnRouteMovementComponent::SetRoutePoints(const std::vector<glm::vec3> &routePoints)
    {
        mRoutePoints = routePoints;
        if (mRoutePoints.size())
        {
            CalculateRouteTotalDistance();
        }
    }

    void OnRouteMovementComponent::CalculateRouteTotalDistance()
    {
        const auto routePointsCount = mRoutePoints.size();
        for (int i = 1; i < routePointsCount; ++i)
        {
            mRouteTotalDistance += glm::length(mRoutePoints[i] - mRoutePoints[i - 1]);
        }
    }

    const std::vector<glm::vec3> &OnRouteMovementComponent::GetRoutePoints() const
    {
        return mRoutePoints;
    }

    void OnRouteMovementComponent::TeleportToDistanceOnRoute(const float distance)
    {
    }

    float OnRouteMovementComponent::GetCurrentDistanceOnRoute() const
    {
        return mCurrentDistanceOnRoute;
    }

    void OnRouteMovementComponent::Move(const float deltaTime)
    {
        if (mIsMovementAllowed)
        {
            if (const auto &actorRootComponentSp = m_actorRootComponent.lock())
            {
                const float distanceToBeDone = mCurrentSpeed * deltaTime;
                float distanceAlreadyDone = mCurrentDistanceOnRoute * mRouteTotalDistance + distanceToBeDone;
                distanceAlreadyDone = std::fmod(distanceAlreadyDone, mRouteTotalDistance);
                const float prctDistanceDone = EngineMath::FloatsNearEqual(mRouteTotalDistance, 0.0f) ? 0.0f : distanceToBeDone / mRouteTotalDistance;
                mCurrentDistanceOnRoute += prctDistanceDone;
                mCurrentDistanceOnRoute = mCurrentDistanceOnRoute > 1.0f ? 0.0f : mCurrentDistanceOnRoute;

                glm::vec3 pPrev, pNext;
                float distancePrev = 0.0f, distanceNext = 0.0f;
                const auto routePointsCount = mRoutePoints.size();
                float accumulatedDistance = 0.0f;
                for (int i = 1; i < routePointsCount; ++i)
                {
                    const float distanceBetweenPrevAndNow = glm::length(mRoutePoints[i] - mRoutePoints[i - 1]);
                    accumulatedDistance += distanceBetweenPrevAndNow;
                    if (distanceAlreadyDone <= accumulatedDistance)
                    {
                        pPrev = mRoutePoints[i - 1];
                        pNext = mRoutePoints[i];
                        distanceNext = accumulatedDistance;
                        distancePrev = accumulatedDistance - distanceBetweenPrevAndNow;
                        break;
                    }
                }

                const auto currentPosition = EngineMath::LerpVec3(distanceAlreadyDone, distancePrev, distanceNext, pPrev, pNext);

                if (!EngineMath::CheckSimilarityVec3(currentPosition, pPrev))
                {
                    const auto directionVec = glm::normalize(currentPosition - pPrev);
                    const auto yawRad = std::atan2(directionVec.x, directionVec.z);
                    const auto &euelerRotationDeg = actorRootComponentSp->GetAdditionalRotation();
                    actorRootComponentSp->SetAdditionalRotation(glm::vec3(euelerRotationDeg.x, RAD_TO_DEG(yawRad + glm::pi<float>()), euelerRotationDeg.z));
                }
                actorRootComponentSp->SetTranslation(currentPosition);
            }
        }
    }

    bool OnRouteMovementComponent::GetIsMovementAllowed() const
    {
        return mIsMovementAllowed;
    }

    void OnRouteMovementComponent::SetIsMovementAllowed(const bool isAllowed)
    {
        mIsMovementAllowed = isAllowed;
    }
}
