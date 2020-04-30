#include "MeshAnimationData.h"

namespace IO
{
	namespace MeshLoader
	{
		namespace Assimp
		{

			MeshAnimationData::MeshAnimationData(aiAnimation** animations, size_t animationCount)
			{
				if (animationCount > 0)
				{
					for (size_t animationIndex = 0; animationIndex < animationCount; animationIndex++)
					{
						aiAnimation* animation = animations[animationIndex];
						Animations.emplace_back(std::move(AnimationLOADER(*animation)));
					}
				}
			}

			MeshAnimationData::~MeshAnimationData()
			{

			}

		}
	}
}