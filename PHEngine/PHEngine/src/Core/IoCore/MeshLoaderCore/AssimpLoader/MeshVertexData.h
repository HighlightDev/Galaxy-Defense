#pragma once

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <vector>
#include <string>
#include <memory>

#include "VertexLOADER.h"

namespace Io
{
	namespace MeshLoader
	{
		namespace Assimp
		{

			template <int32_t count_bones_influence_vertex>
			struct MeshVertexData
			{
			private:

				const aiScene* m_scene;
				aiMesh** m_meshes;

				void GetMeshVertexData();

				void LoadSkin();

				void LoadSkeleton();

				void FillHierarchyRecursive(aiNode* parentNode, class SkeletonBoneLOADER*& parentBone, int32_t& boneIdCounter);

				std::string GetSkeletonArmatureNodeName(aiNode& rootNode);

				void IterateHierarchy(aiNode& parentNode, int32_t& countChildren);

				aiBone* GetBoneByName(const std::string& name) const;

				void CollectIndices(aiMesh* meshBeingCollected, uint32_t lastIndexBeenInterrupted);

				void TryToCollectSkinInfo(size_t startIndex, aiMesh* meshBeingCollected);

				void CollectBlendables(size_t vertexId, std::vector<VertexLOADER>& blendData, aiMesh* meshBeingCollected);

			public:

				MeshVertexData(const aiScene* scene);
				~MeshVertexData();

            bool bHasIndices = false;
            bool bHasNormals = false;
            bool bHasTextureCoordinates = false;
            bool bHasTangentVertices = false;
            bool bHasAnimation = false;

				std::shared_ptr<std::vector<uint32_t>> Indices;

				std::shared_ptr<std::vector<float>> Verts;

				std::shared_ptr<std::vector<float>> T_Verts;

				std::shared_ptr<std::vector<float>> N_Verts;

				std::shared_ptr<std::vector<float>> Tangent_Verts;

				std::shared_ptr<std::vector<float>> Bitanget_Verts;

				std::shared_ptr<std::vector<float>> BlendWeights;

				std::shared_ptr<std::vector<int32_t>> BlendIndices;

				class SkeletonBoneBaseLOADER* SkeletonRoot;

				inline size_t GetBlendablesCount() const {

					return (BlendWeights->size() / count_bones_influence_vertex);
				}

				void CollectBlendWeightsAndIndices(VertexLOADER& blendInfoVertex, size_t blendableIndex);

				void CleanUp();

			};
		}
	}
}
