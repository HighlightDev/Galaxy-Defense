#include "SphereCollisionTestWithFilterAdapter.h"

#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"

#include <algorithm>

using namespace EngineCore;

namespace EnginePhysics {
SphereCollisionTestWithFilterAdapter::SphereCollisionTestWithFilterAdapter(const float sphereRadius)
    : BulletSphereCollisionTestWithFilter(sphereRadius)
{
}

SphereCollisionTestWithFilterAdapter::SphereCollisionTestWithFilterAdapter(
    const float sphereRadius, const std::vector<std::shared_ptr<PhysicsComponent>>& excludeCollisionComponents)
    : BulletSphereCollisionTestWithFilter(sphereRadius)
{
    Initialize(excludeCollisionComponents);
}

void SphereCollisionTestWithFilterAdapter::SphereCollisionTest(
    const std::shared_ptr<PhysicsWorld>& physWorld, const glm::vec3& translation)
{
    BulletSphereCollisionTestWithFilter::SphereCollisionTest(physWorld->GetWorld(), Converter::glmToBullet(translation));
}

std::vector<const PhysicsDescriptor*> SphereCollisionTestWithFilterAdapter::GetCollisionHitPhysicsDescriptors() const
{
    std::vector<const PhysicsDescriptor*> result;
    for (const auto& collisionObj : mCollisionResult) {
        if (const auto& collidedObjDescriptor = reinterpret_cast<const PhysicsDescriptor*>(collisionObj->getUserPointer())) {
            result.emplace_back(collidedObjDescriptor);
        }
    }
    return result;
}

void SphereCollisionTestWithFilterAdapter::Initialize(
    const std::vector<std::shared_ptr<PhysicsComponent>>& excludeCollisionComponents)
{
    if (excludeCollisionComponents.size()) {
        std::vector<btCollisionObject*> excludeCollisionObjects;
        auto insertPosition = excludeCollisionObjects.begin();
        for (const auto& collisionComponent : excludeCollisionComponents) {
            auto collisionObjects = collisionComponent->GetDescriptor()->GetCollisionObjects();
            if (collisionObjects.size()) {
                insertPosition = excludeCollisionObjects.insert(insertPosition, collisionObjects.begin(), collisionObjects.end());
            }
        }

        mExcludeFilterBodies = std::move(excludeCollisionObjects);
    }
}

bool SphereCollisionTestWithFilterAdapter::HasHit() const
{
    return !mCollisionResult.empty();
}
} // namespace EnginePhysics