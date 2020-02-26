#include "AnimationLOADER.h"

#include <math.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Io
{
	namespace MeshLoader
	{
		namespace Assimp
		{

			AnimationLOADER::AnimationLOADER(aiAnimation& animation)
			{
				Name = std::move(std::string(animation.mName.C_Str()));
				AnimationDuration = animation.mDuration;

				size_t boneAnimationChannelsCount = animation.mNumChannels;
				for (size_t channelIndex = 0; channelIndex < boneAnimationChannelsCount; channelIndex++)
				{
					aiNodeAnim* channel = animation.mChannels[channelIndex];
					BoneFrameCollectionLOADER boneFrame(channel->mNodeName.C_Str());

					size_t positionKeysCount = channel->mNumPositionKeys;
					size_t rotationKeysCount = channel->mNumRotationKeys;
					size_t scaleKeysCount = channel->mNumScalingKeys;

					size_t keysNumber = std::max(positionKeysCount, std::max(rotationKeysCount, scaleKeysCount));

					for (size_t keyIndex = 0; keyIndex < keysNumber; keyIndex++)
					{
						glm::vec3 translation, scale;
						glm::quat rotation;
						double time = -1.0;

						if (positionKeysCount > keyIndex)
						{
							const auto& position = channel->mPositionKeys[keyIndex].mValue;
							translation = std::move(glm::vec3(position.x, position.y, position.z));
							time = channel->mPositionKeys[keyIndex].mTime;
						}
						if (scaleKeysCount > keyIndex)
						{
							const auto& scaling = channel->mScalingKeys[keyIndex].mValue;
							scale = std::move(glm::vec3(scaling.x, scaling.y, scaling.z));
							time = channel->mPositionKeys[keyIndex].mTime;
						}
						if (rotationKeysCount > keyIndex)
						{
							const auto& rotating = channel->mRotationKeys[keyIndex].mValue;
							rotation.x = rotating.x;
							rotation.y = rotating.y;
							rotation.z = rotating.z;
                     rotation.w = rotating.w;
							time = channel->mRotationKeys[keyIndex].mTime;
						}

						boneFrame.AddFrame(std::move(AnimationFrameLOADER(time, std::move(translation), std::move(rotation), std::move(scale))));
					}

					FramesBoneCollection.emplace_back(std::move(boneFrame));
				}
			}


			AnimationLOADER::~AnimationLOADER()
			{
			}

		}
	}
}
