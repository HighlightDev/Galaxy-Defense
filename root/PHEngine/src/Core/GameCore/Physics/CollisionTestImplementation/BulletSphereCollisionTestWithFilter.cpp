#include "BulletSphereCollisionTestWithFilter.h"

#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"

#include <algorithm>

using namespace EngineCore;

namespace EnginePhysics {
BulletSphereCollisionTestWithFilter::BulletSphereCollisionTestWithFilter(const float sphereRadius)
    : btCollisionWorld::ContactResultCallback()
    , mSphereShape(std::make_unique<btSphereShape>(sphereRadius))
    , mGhostSphereObject()
    , mExcludeFilterBodies()
    , mCollisionResult()
{
    Initialize();
}

BulletSphereCollisionTestWithFilter::BulletSphereCollisionTestWithFilter(
    const float sphereRadius, std::vector<btCollisionObject*> excludeCollisionObjects)
    : btCollisionWorld::ContactResultCallback()
    , mSphereShape(std::make_unique<btSphereShape>(sphereRadius))
    , mGhostSphereObject()
    , mExcludeFilterBodies(std::move(excludeCollisionObjects))
{
    Initialize();
}

btScalar BulletSphereCollisionTestWithFilter::addSingleResult(
    btManifoldPoint& cp,
    const btCollisionObjectWrapper* colObj0,
    int partId0,
    int index0,
    const btCollisionObjectWrapper* colObj1,
    int partId1,
    int index1)
{
    const auto& collidedObject = colObj1->getCollisionObject();
    if (mGhostSphereObject.get() != collidedObject && collidedObject->getUserPointer()) {
        if (const auto& collidedObjDescriptor = reinterpret_cast<const PhysicsDescriptor*>(collidedObject->getUserPointer())) {
            if (collidedObjDescriptor->GetIsCollisionEnabled()) {
                const auto isExcludedBody = std::any_of(
                    mExcludeFilterBodies.cbegin(), mExcludeFilterBodies.cend(), [collidedObject](const auto& excludeCollObj) {
                        return collidedObject == excludeCollObj;
                    });
                if (!isExcludedBody) {
                    mCollisionResult.emplace_back(collidedObject);
                }
            }
        }
    }

    return 1.0f; // continue collecting collided objects
}

void BulletSphereCollisionTestWithFilter::SphereCollisionTest(btDiscreteDynamicsWorld* physWorld, const btVector3& sphereOrigin)
{
    const auto& collisionObjectTransform = btTransform(btQuaternion(1.0f, 0.0f, 0.0f, 0.0f), sphereOrigin);
    mGhostSphereObject->setWorldTransform(collisionObjectTransform);
    physWorld->contactTest(mGhostSphereObject.get(), *this);
}

bool BulletSphereCollisionTestWithFilter::IsSphereHitCollision() const
{
    return mCollisionResult.size() > 0;
}

std::vector<const btCollisionObject*> BulletSphereCollisionTestWithFilter::GetCollisionHitObjects() const
{
    return mCollisionResult;
}

void BulletSphereCollisionTestWithFilter::Initialize()
{
    mGhostSphereObject = std::make_unique<btPairCachingGhostObject>();

    mGhostSphereObject->setCollisionShape(mSphereShape.get());
    mGhostSphereObject->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
}
} // namespace EnginePhysics