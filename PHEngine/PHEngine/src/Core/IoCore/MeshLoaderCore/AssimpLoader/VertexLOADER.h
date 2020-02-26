#pragma once

#include <stdint.h>
#include <assimp/mesh.h>
#include <vector>
#include <tuple>
#include <utility>

namespace Io
{
	namespace MeshLoader
	{
		namespace Assimp
		{

			struct VertexLOADER
			{

				int32_t m_index;
				std::vector<std::pair<std::tuple<aiBone*, int32_t>, float>> BoneWeightMap;

			public:

				VertexLOADER(int32_t index);
				~VertexLOADER();

				void AddBoneWeight(std::pair<std::tuple<aiBone*, int32_t>, float>&& boneInfo);
			};
		}
	}
}
