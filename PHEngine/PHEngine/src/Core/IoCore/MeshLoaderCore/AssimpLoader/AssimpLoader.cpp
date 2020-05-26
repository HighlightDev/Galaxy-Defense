#include "AssimpLoader.h"
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
			template class AssimpLoader<3>;

			template <int32_t count_bones_influence_vertex>
			AssimpLoader<count_bones_influence_vertex>::AssimpLoader(const std::string& modelFilePath)
			{
            m_scene = importer.ReadFile(modelFilePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
               //aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_Debone | aiProcess_LimitBoneWeights);

            assert((m_scene));

            LoadMeshAndAnimations();
			}

			template <int32_t count_bones_influence_vertex>
			AssimpLoader<count_bones_influence_vertex>::~AssimpLoader()
			{
				m_meshData = nullptr;
				m_meshAninationData = nullptr;
			}

         template <int32_t count_bones_influence_vertex>
         void AssimpLoader<count_bones_influence_vertex>::LoadMeshAndAnimations()
         {
            m_meshData = new MeshData<count_bones_influence_vertex>(m_scene);

            if (m_scene->HasAnimations())
            {
               m_meshAninationData = new AnimationData(m_scene->mAnimations, m_scene->mNumAnimations, m_meshData->GetValidBoneMapping());
            }
         }

			template <int32_t count_bones_influence_vertex>
			MeshData<count_bones_influence_vertex>* AssimpLoader<count_bones_influence_vertex>::GetMeshData() const
			{
				return m_meshData;
			}

			template <int32_t count_bones_influence_vertex>
			AnimationData* AssimpLoader<count_bones_influence_vertex>::GetAnimationData() const
			{
				return m_meshAninationData;
			}
		}
	}
}