#pragma once

#include <string>
#include <vector>
#include <assimp/anim.h>
#include <map>

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

            AnimationLOADER(aiAnimation* animation);

				~AnimationLOADER();
			};

		}

	}
}
