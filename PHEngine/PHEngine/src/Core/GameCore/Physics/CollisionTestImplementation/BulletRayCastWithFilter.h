#pragma once

#include <BulletPhys/btBulletDynamicsCommon.h>
#include <BulletPhys/btBulletCollisionCommon.h>
#include <BulletPhys/BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <vector>

namespace EnginePhysics
{
    class BulletRayCastWithFilter : public btCollisionWorld::ClosestRayResultCallback
    {
    protected:
        std::vector<btCollisionObject *> mExcludeFilterBodies;

    public:
        BulletRayCastWithFilter();

        explicit BulletRayCastWithFilter(std::vector<btCollisionObject *> excludeCollisionObjects);

        void RayTest(const btDiscreteDynamicsWorld *physWorld, const btVector3 &rayFromPosition, const btVector3 &rayToPosition);

        bool IsRayHitCollision() const;

        const btCollisionObject *GetCollisionHitObject() const;

    private:
        btScalar addSingleResult(btCollisionWorld::LocalRayResult &rayResult, bool normalInWorldSpace) override;
    };
}