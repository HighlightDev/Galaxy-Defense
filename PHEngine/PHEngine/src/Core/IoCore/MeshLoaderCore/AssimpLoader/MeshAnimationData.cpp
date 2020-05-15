#include "MeshAnimationData.h"

namespace IO
{
	namespace MeshLoader
	{
		namespace Assimp
		{

			MeshAnimationData::MeshAnimationData(aiAnimation** animations, size_t animationCount, const std::set<std::string>& validBones)
			{
				if (animationCount > 0)
				{
					for (size_t animationIndex = 0; animationIndex < animationCount; animationIndex++)
					{
						aiAnimation* animation = animations[animationIndex];
						Animations.emplace_back(AnimationLOADER(animation, validBones));
					}
				}
			}

			MeshAnimationData::~MeshAnimationData()
			{

			}

		}
	}
}