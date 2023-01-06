#include "GlmToBulletConverter.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

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

   bool isEqual(const btTransform& left, const btTransform& right)
   {
      const btVector3& originL = left.getOrigin();
      const btQuaternion& basisL = left.getRotation();
      const btVector3& originR = right.getOrigin();
      const btQuaternion& basisR = right.getRotation();

      return (
         FloatsNearEqual(originL.getX(), originR.getX()) &&
         FloatsNearEqual(originL.getY(), originR.getY()) &&
         FloatsNearEqual(originL.getZ(), originR.getZ()) &&
         FloatsNearEqual(basisL.getX(), basisR.getX()) &&
         FloatsNearEqual(basisL.getY(), basisR.getY()) &&
         FloatsNearEqual(basisL.getZ(), basisR.getZ()) &&
         FloatsNearEqual(basisL.getW(), basisR.getW()));
   }
}