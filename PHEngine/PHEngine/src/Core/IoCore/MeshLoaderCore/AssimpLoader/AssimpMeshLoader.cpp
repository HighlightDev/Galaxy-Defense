#include "AssimpMeshLoader.h"
#include "Core/CommonCore/Assertion.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>

namespace IO
{
	namespace MeshLoader
	{
		namespace Assimp
		{
			template class AssimpMeshLoader<3>;

			template <int32_t count_bones_influence_vertex>
			AssimpMeshLoader<count_bones_influence_vertex>::AssimpMeshLoader(const std::string& modelFilePath)
			{
				m_scene = importer.ReadFile(modelFilePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_Debone | aiProcess_LimitBoneWeights);
            assert((m_scene));

            LoadMeshAndAnimations();
			}

			template <int32_t count_bones_influence_vertex>
			AssimpMeshLoader<count_bones_influence_vertex>::~AssimpMeshLoader()
			{
				m_meshData = nullptr;
				m_meshAninationData = nullptr;
			}

         template <int32_t count_bones_influence_vertex>
         void AssimpMeshLoader<count_bones_influence_vertex>::LoadMeshAndAnimations()
         {
            m_meshData = new MeshVertexData<count_bones_influence_vertex>(m_scene);

            if (m_scene->HasAnimations())
            {
               m_meshAninationData = new MeshAnimationData(m_scene->mAnimations, m_scene->mNumAnimations, m_meshData->GetValidBoneSet());
            }
         }

			template <int32_t count_bones_influence_vertex>
			MeshVertexData<count_bones_influence_vertex>* AssimpMeshLoader<count_bones_influence_vertex>::GetMeshData() const
			{
				return m_meshData;
			}

			template <int32_t count_bones_influence_vertex>
			MeshAnimationData* AssimpMeshLoader<count_bones_influence_vertex>::GetAnimationData() const
			{
				return m_meshAninationData;
			}
		}
	}
}