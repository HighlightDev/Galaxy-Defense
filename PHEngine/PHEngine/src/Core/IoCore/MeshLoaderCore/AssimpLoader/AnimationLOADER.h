#pragma once

#include <string>
#include <vector>
#include <assimp/anim.h>

#include "BoneFrameCollectionLOADER.h"

namespace Io
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

				AnimationLOADER(aiAnimation& animation);

				~AnimationLOADER();
			};

		}

	}
}
