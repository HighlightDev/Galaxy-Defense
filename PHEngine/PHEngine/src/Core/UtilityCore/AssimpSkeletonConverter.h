#pragma once

#include <assimp/matrix4x4.h>
#include <glm/mat4x4.hpp>
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

	private:

		static glm::mat4 ConvertAssimpMatrix4x4ToGlmMat4(const aiMatrix4x4& srcMatrix);

		static void IterateBoneTree(Bone* dstParentBone, SkeletonBoneLOADER* srcParentNode);
	};

}

