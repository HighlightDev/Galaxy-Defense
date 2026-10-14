#pragma once

#include "CollisionShapeBase.h"

#include <glm/vec3.hpp>

namespace EnginePhysics {

struct CollisionPlaneShape : public CollisionShapeBase {
    CollisionPlaneShape(const glm::vec3& normal, const double d)
        : CollisionShapeBase(new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), d))
    {
    }
};

} // namespace EnginePhysics
