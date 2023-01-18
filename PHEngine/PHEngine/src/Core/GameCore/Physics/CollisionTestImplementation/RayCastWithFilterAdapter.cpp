#include "RayCastWithFilterAdapter.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"

#include <algorithm>

namespace EnginePhysics
{
    RayCastWithFilterAdapter::RayCastWithFilterAdapter()
        : BulletRayCastWithFilter()
    {
    }

    RayCastWithFilterAdapter::RayCastWithFilterAdapter(std::vector<std::shared_ptr<PhysicsComponent>> excludeCollisionComponents)
        : BulletRayCastWithFilter()
    {
        Initialize(std::move(excludeCollisionComponents));
    }

    void RayCastWithFilterAdapter::Initialize(std::vector<std::shared_ptr<PhysicsComponent>> excludeCollisionComponents)
    {
        if (excludeCollisionComponents.size())
        {
            std::vector<btCollisionObject *> excludeCollisionObjects;
            auto insertPosition = excludeCollisionObjects.begin();
            for (const auto &collisionComponent : excludeCollisionComponents)
            {
                auto collisionObjects = collisionComponent->GetDescriptor()->GetCollisionObjects();
                if (collisionObjects.size())
                {
                    insertPosition = excludeCollisionObjects.insert(insertPosition, collisionObjects.begin(), collisionObjects.end());
                }
            }

            mExcludeFilterBodies = std::move(excludeCollisionObjects);
        }
    }

    void RayCastWithFilterAdapter::RayTest(const PhysicsWorld *physWorld, const glm::vec3 &rayFromPosition, const glm::vec3 &rayToPosition)
    {
        BulletRayCastWithFilter::RayTest(physWorld->GetWorld(), Converter::glmToBullet(rayFromPosition), Converter::glmToBullet(rayToPosition));
    }

    const PhysicsDescriptor *RayCastWithFilterAdapter::GetCollisionHitPhysicsDescriptor() const
    {
        PhysicsDescriptor *collidedPhysicsDescriptor = nullptr;

        if (GetCollisionHitObject() && GetCollisionHitObject()->getUserPointer())
        {
            collidedPhysicsDescriptor = static_cast<PhysicsDescriptor *>(GetCollisionHitObject()->getUserPointer());
        }

        return collidedPhysicsDescriptor;
    }
}