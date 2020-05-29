#pragma once

#include "MeshData.h"
#include "AnimationData.h"

#include <assimp/Importer.hpp>

namespace IO
{
	namespace MeshLoader
	{
		namespace Assimp
		{
			template <int32_t count_bones_influence_vertex>
			class AssimpLoader
			{
				typename ::Assimp::Importer importer;
				const struct aiScene* m_scene;

				MeshData<count_bones_influence_vertex>* m_meshData;
				AnimationData* m_meshAninationData;
            AnimatedMeshData* m_animatedMeshData;
            MeshAttributes* m_meshAttributes;

			public:

				AssimpLoader(const std::string& modelFilePath);

				~AssimpLoader();

				MeshData<count_bones_influence_vertex>* GetMeshData() const;

				AnimationData* GetAnimationData() const;

            AnimatedMeshData* GetAnimatedMeshData() const;

            MeshAttributes* GetMeshAttributes() const;

         private:

            void LoadMeshAndAnimations();
			};
		}
	}
}
