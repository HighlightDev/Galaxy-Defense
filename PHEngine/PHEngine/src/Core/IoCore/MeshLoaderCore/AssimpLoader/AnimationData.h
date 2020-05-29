#pragma once

#include <assimp/anim.h>
#include <vector>
#include <map>
#include <string>

#include "AnimationLOADER.h"

namespace IO
{
	namespace MeshLoader
	{
		namespace Assimp
		{
			class AnimationData
			{

				std::vector<AnimationLOADER> Animations;

			public:

				AnimationData(aiAnimation** animations, size_t animationCount);

				~AnimationData();

            inline const std::vector<AnimationLOADER>& GetAnimations() const {

               return Animations;
            }
			};

		}
	}
}
