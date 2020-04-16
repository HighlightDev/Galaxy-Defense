#include "GlmToBulletConverter.h"

namespace Converter
{
   glm::quat bulletToGlm(const btQuaternion& q)
   {
      return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ());
   }

   btQuaternion glmToBullet(const glm::quat& q) {
      return btQuaternion(q.x, q.y, q.z, q.w);
   }

   glm::vec3 bulletToGlm(const btVector3& vec)
   {
      return glm::vec3(vec.getX(), vec.getY(), vec.getZ());
   }

   btVector3 glmToBullet(const glm::vec3& vec) {
      return btVector3(vec.x, vec.y, vec.z);
   }
}