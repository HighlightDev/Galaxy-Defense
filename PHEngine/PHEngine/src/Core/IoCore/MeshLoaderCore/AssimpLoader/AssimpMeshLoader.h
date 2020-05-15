#pragma once

#include "MeshVertexData.h"
#include "MeshAnimationData.h"

#include <assimp/Importer.hpp>

namespace IO
{
	namespace MeshLoader
	{
		namespace Assimp
		{
			template <int32_t count_bones_influence_vertex>
			class AssimpMeshLoader
			{
				typename ::Assimp::Importer importer;
				const struct aiScene* m_scene;

				MeshVertexData<count_bones_influence_vertex>* m_meshData;
				MeshAnimationData* m_meshAninationData;

			public:

				AssimpMeshLoader(const std::string& modelFilePath);

				~AssimpMeshLoader();

				MeshVertexData<count_bones_influence_vertex>* GetMeshData() const;

				MeshAnimationData* GetAnimationData() const;

         private:

            void LoadMeshAndAnimations();
			};
		}
	}
}
