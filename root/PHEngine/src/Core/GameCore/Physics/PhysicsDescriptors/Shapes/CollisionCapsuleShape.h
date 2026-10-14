#pragma once

#include "CollisionShapeBase.h"

namespace EnginePhysics {

struct CollisionCapsuleShape : public CollisionShapeBase {
    CollisionCapsuleShape(const double radius, const double height)
        : CollisionShapeBase(new btCapsuleShape(radius, height))
    {
    }

    float GetRadius() const
    {
        return static_cast<btCapsuleShape*>(mCollisionShape)->getRadius();
    }

    float GetHeight() const
    {
        return static_cast<btCapsuleShape*>(mCollisionShape)->getHalfHeight() * 2.0f;
    }
};

} // namespace EnginePhysics