#pragma once

#include "BulletSphereCollisionTestWithFilter.h"

#include <vector>
#include <memory>
#include <glm/vec3.hpp>

namespace EnginePhysics
{
    class PhysicsDescriptor;
    class PhysicsWorld;
    class PhysicsComponent;

    class SphereCollisionTestWithFilterAdapter : public BulletSphereCollisionTestWithFilter
    {
    public:
        SphereCollisionTestWithFilterAdapter(const float sphereRadius);

        explicit SphereCollisionTestWithFilterAdapter(const float sphereRadius, std::vector<std::shared_ptr<PhysicsComponent>> excludeCollisionComponents);

        void SphereCollisionTest(PhysicsWorld *physWorld, const glm::vec3 &translation);

        std::vector<const PhysicsDescriptor *> GetCollisionHitPhysicsDescriptors() const;

    private:
        void Initialize(std::vector<std::shared_ptr<PhysicsComponent>> excludeCollisionComponents);
    };
}