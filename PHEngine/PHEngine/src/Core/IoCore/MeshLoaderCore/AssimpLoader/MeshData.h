#pragma once

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <glm/mat4x4.hpp>

#include "VertexLOADER.h"

namespace IO
{
	namespace MeshLoader
	{
		namespace Assimp
		{
         struct MeshNode
         {
            std::string Name;
            std::vector<MeshNode*> Children;
            glm::mat4 NodeTransformation;
         };

         struct MeshBoneInfo
         {
            glm::mat4 BoneOffset;

            MeshBoneInfo(const glm::mat4& boneOffset)
               : BoneOffset(BoneOffset) 
            {
            }
         };

         struct Collector
         {
            const aiScene* mScene;

            std::map<std::string, MeshNode*> MeshNodeMapping;
            MeshNode* meshRootNode = nullptr;

            std::map<std::string, MeshBoneInfo> BoneMapping;

            using NodeAnimationSequence_t = std::map<std::string, /*stub*/int>;

            std::map<std::string, NodeAnimationSequence_t> AnimationMapping;

            Collector(const aiScene* scene);

            void Collect();


         private:
            void CollectNodeHierarchy(const aiNode* pNode, const MeshNode* meshNode);

            void CollectBones();

            void CollectAnimation();

            void AnimationIterateNodes(const aiAnimation* pAnimation, const aiNode* pNode);
         };


			template <int32_t count_bones_influence_vertex>
			struct MeshData
			{
			private:

				const aiScene* m_scene;
				aiMesh** m_meshes;

            std::set<std::string> NodeNames;
            std::map<std::string, size_t> BoneMaping;

				void GetMeshData();

				void LoadSkin();

				void LoadSkeleton();

				void FillHierarchyRecursive(aiNode* parentNode, class SkeletonBoneLOADER*& parentBone, int32_t& boneIdCounter);

				aiString GetSkeletonArmatureNodeName(aiNode& rootNode);

				void IterateHierarchy(aiNode& parentNode, int32_t& countChildren);

				aiBone* GetBoneByName(const aiString& name) const;

				void CollectIndices(aiMesh* meshBeingCollected, uint32_t lastIndexBeenInterrupted);

				void TryToCollectSkinInfo(size_t startIndex, aiMesh* meshBeingCollected);

				void CollectBlendables(size_t vertexId, std::vector<VertexLOADER>& blendData, aiMesh* meshBeingCollected);


            void NEW_CollectBoneInfo();

			public:

				MeshData(const aiScene* scene);
				~MeshData();

            bool bHasIndices = false;
            bool bHasNormals = false;
            bool bHasTextureCoordinates = false;
            bool bHasTangentVertices = false;
            bool bHasAnimation = false;

				std::vector<uint32_t> Indices;

				std::vector<float> Verts;

				std::vector<float> T_Verts;

				std::vector<float> N_Verts;

				std::vector<float> Tangent_Verts;

				std::vector<float> Bitanget_Verts;

				std::vector<float> BlendWeights;

				std::vector<int32_t> BlendIndices;

				class SkeletonBoneBaseLOADER* SkeletonRoot;

				inline size_t GetBlendablesCount() const {

					return (BlendWeights.size() / count_bones_influence_vertex);
				}

				void CollectBlendWeightsAndIndices(VertexLOADER& blendInfoVertex, size_t blendableIndex);

				void CleanUp();

            std::map<std::string, size_t> GetValidBoneMapping() const;

			};
		}
	}
}
