#include "AssimpMeshLoader.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>

namespace Io
{
	namespace MeshLoader
	{
		namespace Assimp
		{
			template class AssimpMeshLoader<3>;

			template <int32_t count_bones_influence_vertex>
			AssimpMeshLoader<count_bones_influence_vertex>::AssimpMeshLoader(std::string& modelFilePath)
			{
				m_scene = importer.ReadFile(modelFilePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
				if (!m_scene)
					throw std::exception("Could not load scene from file!");
			}

			template <int32_t count_bones_influence_vertex>
			AssimpMeshLoader<count_bones_influence_vertex>::~AssimpMeshLoader()
			{
				delete m_meshData;
				m_meshData = nullptr;

				delete m_meshAninationData;
				m_meshAninationData = nullptr;
			}

			template <int32_t count_bones_influence_vertex>
			bool AssimpMeshLoader<count_bones_influence_vertex>::GetHasAnimationData() const {

				return m_scene->HasAnimations();
			}


			template <int32_t count_bones_influence_vertex>
			MeshVertexData<count_bones_influence_vertex>& AssimpMeshLoader<count_bones_influence_vertex>::GetMeshData()
			{
				if (!m_meshData)
				{
					m_meshData = new MeshVertexData<count_bones_influence_vertex>(m_scene);
				}

				return *m_meshData;
			}

			template <int32_t count_bones_influence_vertex>
			MeshAnimationData& AssimpMeshLoader<count_bones_influence_vertex>::GetAnimationData()
			{
				if (!m_meshAninationData)
					m_meshAninationData = new MeshAnimationData(m_scene->mAnimations, m_scene->mNumAnimations);

				return *m_meshAninationData;
			}
		}
	}
}