#pragma once

#include "CollisionShapeBase.h"

namespace EnginePhysics {

struct CollisionSphereShape : public CollisionShapeBase {

    CollisionSphereShape(const double radius)
        : CollisionShapeBase(new btSphereShape(radius))
    {
    }

    double GetRadius() const
    {
        return static_cast<btSphereShape*>(mCollisionShape)->getRadius();
    }
};

} // namespace EnginePhysics
