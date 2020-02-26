#pragma once

#include <assimp/anim.h>
#include <vector>

#include "AnimationLOADER.h"

namespace Io
{
	namespace MeshLoader
	{
		namespace Assimp
		{
			class MeshAnimationData
			{

				std::vector<AnimationLOADER> Animations;

			public:

				MeshAnimationData(aiAnimation** animations, size_t animationCount);

				~MeshAnimationData();

            inline const std::vector<AnimationLOADER>& GetAnimations() const {

               return Animations;
            }
			};

		}
	}
}
