#pragma once

#include <assimp/anim.h>
#include <vector>
#include <set>
#include <string>

#include "AnimationLOADER.h"

namespace IO
{
	namespace MeshLoader
	{
		namespace Assimp
		{
			class MeshAnimationData
			{

				std::vector<AnimationLOADER> Animations;

			public:

				MeshAnimationData(aiAnimation** animations, size_t animationCount, const std::set<std::string>& validBones);

				~MeshAnimationData();

            inline const std::vector<AnimationLOADER>& GetAnimations() const {

               return Animations;
            }
			};

		}
	}
}
