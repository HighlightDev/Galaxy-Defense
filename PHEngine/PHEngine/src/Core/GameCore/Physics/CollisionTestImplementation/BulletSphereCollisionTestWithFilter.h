#pragma once

#include <BulletPhys/BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletPhys/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletPhys/btBulletCollisionCommon.h>
#include <BulletPhys/btBulletDynamicsCommon.h>
#include <glm/vec3.hpp>

#include <memory>
#include <vector>

namespace EnginePhysics {
class BulletSphereCollisionTestWithFilter : public btCollisionWorld::ContactResultCallback {
protected:
    std::unique_ptr<btSphereShape> mSphereShape;

    std::unique_ptr<btPairCachingGhostObject> mGhostSphereObject;

    std::vector<btCollisionObject*> mExcludeFilterBodies;

    std::vector<const btCollisionObject*> mCollisionResult;

public:
    BulletSphereCollisionTestWithFilter(const float sphereRadius);

    explicit BulletSphereCollisionTestWithFilter(
        const float sphereRadius, std::vector<btCollisionObject*> excludeCollisionObjects);

    void SphereCollisionTest(btDiscreteDynamicsWorld* physWorld, const btVector3& sphereOrigin);

    bool IsSphereHitCollision() const;

    std::vector<const btCollisionObject*> GetCollisionHitObjects() const;

private:
    btScalar addSingleResult(
        btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0,
        int partId0,
        int index0,
        const btCollisionObjectWrapper* colObj1,
        int partId1,
        int index1) override;

    void Initialize();
};
} // namespace EnginePhysics