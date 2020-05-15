#pragma once

#include <string>
#include <vector>
#include <assimp/anim.h>
#include <set>

#include "BoneFrameCollectionLOADER.h"

namespace IO
{
	namespace MeshLoader
	{

		namespace Assimp
		{

			struct AnimationLOADER
			{
				double AnimationDuration;
				std::string Name;
				std::vector<BoneFrameCollectionLOADER> FramesBoneCollection;

			public:

            AnimationLOADER(aiAnimation* animation, const std::set<std::string>& validBones);

				~AnimationLOADER();
			};

		}

	}
}
