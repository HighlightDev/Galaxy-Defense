#include "VertexLOADER.h"

namespace Io
{
	namespace MeshLoader
	{
		namespace Assimp
		{

			VertexLOADER::VertexLOADER(int32_t index)
				: m_index(index)
			{
			}


			VertexLOADER::~VertexLOADER()
			{
			}


			void VertexLOADER::AddBoneWeight(std::pair<std::tuple<aiBone*, int32_t>, float>&& boneInfo)
			{
				BoneWeightMap.emplace_back(boneInfo);
			}
		}
	}
}