#pragma once

#include "BulletRayCastWithFilter.h"

#include <glm/vec3.hpp>

#include <memory>
#include <vector>

namespace EnginePhysics {
class PhysicsDescriptor;
class PhysicsWorld;
class PhysicsComponent;

class RayCastWithFilterAdapter : public BulletRayCastWithFilter {
public:
    RayCastWithFilterAdapter();

    explicit RayCastWithFilterAdapter(std::vector<std::shared_ptr<PhysicsComponent>> excludeCollisionComponents);

    void
    RayTest(const std::shared_ptr<PhysicsWorld>& physWorld, const glm::vec3& rayFromPosition, const glm::vec3& rayToPosition);

    const PhysicsDescriptor* GetCollisionHitPhysicsDescriptor() const;

private:
    void Initialize(std::vector<std::shared_ptr<PhysicsComponent>> excludeCollisionComponents);
};
} // namespace EnginePhysics