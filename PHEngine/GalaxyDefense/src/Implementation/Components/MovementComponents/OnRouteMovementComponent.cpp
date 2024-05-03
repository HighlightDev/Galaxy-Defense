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
        const auto newRoutePointsSize = routePoints.size();
        if (newRoutePointsSize)
        {
            float totalDistance = 0.0f;
            for (int i = 0; i < newRoutePointsSize; ++i)
            {
                if (0 == i)
                {
                    mRoutePoints.emplace_back(std::make_pair(0.0f, routePoints.at(0)));
                }
                else
                {
                    const auto nowAndPrevPointsDistance = glm::length(routePoints[i] - routePoints[i - 1]);
                    totalDistance += nowAndPrevPointsDistance;
                    mRoutePoints.emplace_back(std::make_pair(totalDistance, routePoints[i]));
                }
            }
            mRouteTotalDistance = totalDistance;
        }
    }

    std::vector<glm::vec3> OnRouteMovementComponent::GetRoutePoints() const
    {
        std::vector<glm::vec3> result;
        result.reserve(mRoutePoints.size());
        std::transform(mRoutePoints.begin(), mRoutePoints.end(), std::back_inserter(result), [](const auto &routePair)
                       { return routePair.second; });
        return result;
    }

    void OnRouteMovementComponent::TeleportToMovementProgressOnRoute(const float seekDistance)
    {
        if (const auto &actorRootComponentSp = m_actorRootComponent.lock())
        {
            if (seekDistance <= mRouteTotalDistance && seekDistance >= 0.0f)
            {
                std::pair<float, glm::vec3> pPrev, pNext;
                const auto routePointsCount = mRoutePoints.size();
                for (int i = 1; i < routePointsCount; ++i)
                {
                    const auto &prevRoutePoint = mRoutePoints[i - 1];
                    const auto &currentRoutePoint = mRoutePoints[i];
                    if (currentRoutePoint.first >= seekDistance && prevRoutePoint.first <= seekDistance)
                    {
                        pNext = currentRoutePoint;
                        pPrev = prevRoutePoint;
                        break;
                    }
                }

                const auto currentPosition = EngineMath::LerpVec3(seekDistance, pPrev.first, pNext.first, pPrev.second, pNext.second);

                if (!EngineMath::CheckSimilarityVec3(currentPosition, pPrev.second))
                {
                    const auto directionVec = glm::normalize(currentPosition - pPrev.second);
                    const auto yawRad = std::atan2(directionVec.x, directionVec.z);
                    const auto &euelerRotationDeg = actorRootComponentSp->GetAdditionalRotation();
                    actorRootComponentSp->SetAdditionalRotation(glm::vec3(euelerRotationDeg.x, RAD_TO_DEG(yawRad + glm::pi<float>()), euelerRotationDeg.z));
                }
                actorRootComponentSp->SetTranslation(currentPosition);
            }
        }
    }

    float OnRouteMovementComponent::GetMovementProgressOnRoute() const
    {
        return mMovementProgressOnRoute;
    }

    void OnRouteMovementComponent::Move(const float deltaTime)
    {
        if (mIsMovementAllowed)
        {
            const float distanceToBeDone = mCurrentSpeed * deltaTime;
            float distanceAlreadyDone = mMovementProgressOnRoute * mRouteTotalDistance + distanceToBeDone;
            distanceAlreadyDone = std::fmod(distanceAlreadyDone, mRouteTotalDistance);
            const float prctDistanceDone = EngineMath::FloatsNearEqual(mRouteTotalDistance, 0.0f) ? 0.0f : distanceToBeDone / mRouteTotalDistance;
            mMovementProgressOnRoute += prctDistanceDone;
            if (mMovementProgressOnRoute > 1.0f)
            {
                mIsDistanceCompleted = true;
                mMovementProgressOnRoute -= 1.0f;
            }

            TeleportToMovementProgressOnRoute(distanceAlreadyDone);
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

    bool OnRouteMovementComponent::GetIsDistanceCompleted() const
    {
        return mIsDistanceCompleted;
    }

    void OnRouteMovementComponent::ResetStates()
    {
        mIsDistanceCompleted = false;
        mIsMovementAllowed = false;
        mMovementProgressOnRoute = 0.0f;
        mRouteTotalDistance = 0.0f;
    }
}
