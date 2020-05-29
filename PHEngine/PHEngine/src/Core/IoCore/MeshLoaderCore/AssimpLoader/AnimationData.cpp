#include "AnimationData.h"

namespace IO
{
	namespace MeshLoader
	{
		namespace Assimp
		{

			AnimationData::AnimationData(aiAnimation** animations, size_t animationCount)
			{
				if (animationCount > 0)
				{
					for (size_t animationIndex = 0; animationIndex < animationCount; animationIndex++)
					{
						aiAnimation* animation = animations[animationIndex];
						Animations.emplace_back(AnimationLOADER(animation));
					}
				}
			}

			AnimationData::~AnimationData()
			{

			}

		}
	}
}