#include "BulletRayCastWithFilter.h"

#include <algorithm>

namespace EnginePhysics
{
    BulletRayCastWithFilter::BulletRayCastWithFilter()
        : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0),
                                                     btVector3(0.0, 0.0, 0.0)),
          mExcludeFilterBodies()
    {
    }

    BulletRayCastWithFilter::BulletRayCastWithFilter(std::vector<btCollisionObject *> excludeCollisionObjects)
        : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0),
                                                     btVector3(0.0, 0.0, 0.0)),
          mExcludeFilterBodies(std::move(excludeCollisionObjects))
    {
    }

    btScalar BulletRayCastWithFilter::addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace)
    {
        return std::any_of(mExcludeFilterBodies.cbegin(), mExcludeFilterBodies.cend(), [&rayResult](const auto &excludeCollObj)
                           { return rayResult.m_collisionObject == excludeCollObj; })
                   ? 1.0f
                   : ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
    }

    void BulletRayCastWithFilter::RayTest(const btDiscreteDynamicsWorld *physWorld,
                                    const btVector3 &rayFromPosition,
                                    const btVector3 &rayToPosition)
    {
        physWorld->rayTest(rayFromPosition, rayToPosition, *this);
    }

    bool BulletRayCastWithFilter::IsRayHitCollision() const
    {
        return hasHit();
    }

    const btCollisionObject *BulletRayCastWithFilter::GetCollisionHitObject() const
    {
        return m_collisionObject;
    }
}