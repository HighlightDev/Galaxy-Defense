#pragma once

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <glm/mat4x4.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/vec3.hpp>

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
         };

         /****************************/

         struct FrameRotation
         {
            glm::quat Rotation;
            float Time;
         };

         struct FrameTranslation
         {
            glm::vec3 Translation;
            float Time;
         };

         struct FrameScale
         {
            glm::vec3 Scale;
            float Time;
         };

         struct FrameTransform
         {
            glm::quat Rotation;
            glm::vec3 Translation;
            glm::vec3 Scale;
            float Time;
         };

         struct AnimationSequenceData
         {
            std::vector<FrameRotation> RotationFrames;
            std::vector<FrameTranslation> TranslationFrames;
            std::vector<FrameScale> ScaleFrames;
         };

         struct AnimationMappingData
         {
            using NodeAnimationBinding_t = std::map<std::string /* Node name */, AnimationSequenceData>;

            NodeAnimationBinding_t NodeAnimationBindings;

            float AnimationDuration;
         };

         /****************************/

         struct Collector
         {
            const aiScene* mScene;

            std::map<std::string /* Node Name */, MeshNode*> MeshNodeMapping;
            MeshNode* meshRootNode = nullptr;

            std::map<std::string /* Bone Name */ , MeshBoneInfo> BoneMapping;

  

            std::map<std::string /* Animation Name */, AnimationMappingData> AnimationMapping;

            Collector(const aiScene* scene);

            void Collect();


         private:
            void CollectNodeHierarchy(const aiNode* pNode, MeshNode* meshNode);

            void CollectBones();

            void CollectAnimation();

            void AnimationIterateNodes(const aiAnimation* pAnimation, const aiNode* pNode, AnimationMappingData::NodeAnimationBinding_t& nodeAnimationBindings);
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
