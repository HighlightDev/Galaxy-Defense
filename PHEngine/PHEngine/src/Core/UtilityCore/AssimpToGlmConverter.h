#pragma once

#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <vector>

namespace EngineUtility
{
	class AssimpToGlmConverter
	{

	public:

		static glm::mat4 ConvertAssimpMatrix4x4ToGlmMat4(const aiMatrix4x4& srcMatrix);

      static glm::quat ConvertAssimpQuatToGlmQuat(const aiQuaternion& rotation);

      static glm::vec3 ConvertAssimpVec3ToGlmVec3(const aiVector3D& vector);
	};

}

