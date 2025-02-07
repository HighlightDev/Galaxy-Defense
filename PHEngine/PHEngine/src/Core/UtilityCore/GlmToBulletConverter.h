#pragma once

#include <BulletPhys/btBulletDynamicsCommon.h>
#include <glm/ext/quaternion_float.hpp>

namespace Converter {
glm::quat bulletToGlm(const btQuaternion& q);

btQuaternion glmToBullet(const glm::quat& q);

glm::vec3 bulletToGlm(const btVector3& vec);

btVector3 glmToBullet(const glm::vec3& vec);

bool isEqual(const btTransform& left, const btTransform& right);
} // namespace Converter