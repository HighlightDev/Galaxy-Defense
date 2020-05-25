#pragma once

#include <assimp/matrix4x4.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <vector>

#include "Core/GraphicsCore/Animation/Bone.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/SkeletonBoneLOADER.h"
#include "Core/GraphicsCore/Animation/AnimationSequence.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/AnimationLOADER.h"

using namespace Graphics::Animation;
using namespace IO::MeshLoader::Assimp;

namespace EngineUtility
{

	class AssimpSkeletonConverter
	{

	public:

		static Bone* ConvertAssimpBoneToEngineBone(SkeletonBoneBaseLOADER* rootBone);

      static std::vector<AnimationSequence> ConvertAssimpAnimationToEngineAnimation(const std::vector<AnimationLOADER>& srcAnimations);

      static BoneInfo ConvertAssimpBoneInfoToEngineBoneInfo(aiBone* assimpBone);

		static glm::mat4 ConvertAssimpMatrix4x4ToGlmMat4(const aiMatrix4x4& srcMatrix);

      static glm::quat ConvertAssimpQuatToGlmQuat(const aiQuaternion& rotation);

      static glm::vec3 ConvertAssimpVec3ToGlmVec3(const aiVector3D& vector);

   private:

		static void IterateBoneTree(Bone* dstParentBone, SkeletonBoneLOADER* srcParentNode);
	};

}

