#pragma once

#include "BulletSphereCollisionTestWithFilter.h"

#include <glm/vec3.hpp>

#include <memory>
#include <vector>

namespace EnginePhysics {
class PhysicsDescriptor;
class PhysicsWorld;
class PhysicsComponent;

class SphereCollisionTestWithFilterAdapter : public BulletSphereCollisionTestWithFilter {
public:
    explicit SphereCollisionTestWithFilterAdapter(const float sphereRadius);

    explicit SphereCollisionTestWithFilterAdapter(
        const float sphereRadius, std::vector<std::shared_ptr<PhysicsComponent>> excludeCollisionComponents);

    void SphereCollisionTest(const std::shared_ptr<PhysicsWorld>& physWorld, const glm::vec3& translation);

    std::vector<const PhysicsDescriptor*> GetCollisionHitPhysicsDescriptors() const;

    bool HasHit() const;

private:
    void Initialize(std::vector<std::shared_ptr<PhysicsComponent>> excludeCollisionComponents);
};
} // namespace EnginePhysics