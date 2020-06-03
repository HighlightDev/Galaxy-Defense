#include "AssimpToGlmConverter.h"

namespace EngineUtility
{

	glm::mat4 AssimpToGlmConverter::ConvertAssimpMatrix4x4ToGlmMat4(const aiMatrix4x4& srcMatrix)
	{
		glm::mat4 dstMatrix(srcMatrix.a1, srcMatrix.b1, srcMatrix.c1, srcMatrix.d1,
         srcMatrix.a2, srcMatrix.b2, srcMatrix.c2, srcMatrix.d2,
         srcMatrix.a3, srcMatrix.b3, srcMatrix.c3, srcMatrix.d3,
         srcMatrix.a4, srcMatrix.b4, srcMatrix.c4, srcMatrix.d4);

		return dstMatrix;
	}

   glm::quat AssimpToGlmConverter::ConvertAssimpQuatToGlmQuat(const aiQuaternion& rotation)
   {
      glm::quat result;
      result.x = rotation.x;
      result.y = rotation.y;
      result.z = rotation.z;
      result.w = rotation.w;
      return result;
   }

   glm::vec3 AssimpToGlmConverter::ConvertAssimpVec3ToGlmVec3(const aiVector3D& vector)
   {
      return glm::vec3(vector.x, vector.y, vector.z);
   }
}
