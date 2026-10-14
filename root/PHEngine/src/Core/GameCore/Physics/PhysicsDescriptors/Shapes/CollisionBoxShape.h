#pragma once

#include "CollisionShapeBase.h"
#include "Core/UtilityCore/GlmToBulletConverter.h"

#include <glm/vec3.hpp>

namespace EnginePhysics {

struct CollisionBoxShape : public CollisionShapeBase {
    CollisionBoxShape(const glm::vec3& halfExtent)
        : CollisionShapeBase(new btBoxShape(btVector3(halfExtent.x, halfExtent.y, halfExtent.z)))
    {
    }

    glm::vec3 GetHalfExtent() const
    {
        return Converter::bulletToGlm(static_cast<btBoxShape*>(mCollisionShape)->getHalfExtentsWithoutMargin());
    }
};

} // namespace EnginePhysics