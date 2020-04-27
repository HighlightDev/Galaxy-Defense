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
         CompareFloats(originL.getX(), originR.getX()) &&
         CompareFloats(originL.getY(), originR.getY()) &&
         CompareFloats(originL.getZ(), originR.getZ()) &&
         CompareFloats(basisL.getX(), basisR.getX()) &&
         CompareFloats(basisL.getY(), basisR.getY()) &&
         CompareFloats(basisL.getZ(), basisR.getZ()) &&
         CompareFloats(basisL.getW(), basisR.getW()));
   }
}