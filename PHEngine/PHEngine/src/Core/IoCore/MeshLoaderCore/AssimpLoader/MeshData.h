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
#include <iostream>

#include "VertexLOADER.h"

#define MAX_BONES_PER_VERT 3

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

         struct FrameBase
         {
            float Time;
         };

         struct FrameRotation 
            : public FrameBase
         {
            glm::quat Rotation;
         };

         struct FrameTranslation
            : public FrameBase
         {
            glm::vec3 Translation;
         };

         struct FrameScale
            : public FrameBase
         {
            glm::vec3 Scale;
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

         struct VertexBoneData
         {
            size_t BoneIndices[MAX_BONES_PER_VERT]{ 0 };
            float Weights[MAX_BONES_PER_VERT] { 0.0f };

            bool IsFilled = false;

            void AddBoneData(size_t boneIndex, float weight)
            {
               if (!IsFilled)
               {
                  for (size_t i = 0; i < MAX_BONES_PER_VERT; ++i)
                  {
                     if (Weights[i] <= 0.0005f)
                     {
                        BoneIndices[i] = boneIndex;
                        Weights[i] = weight;
                        IsFilled = (i == (MAX_BONES_PER_VERT - 1));
                        return;
                     }
                  }
               }
            }
         };

         struct Collector
         {
            const aiScene* mScene;

            std::map<std::string /* Node Name */, MeshNode*> MeshNodeMapping;
            MeshNode* meshRootNode = nullptr;

            std::map<std::string /* Bone Name */ , MeshBoneInfo> BoneMapping;

            glm::mat4 GlobalInverseTransform;

            std::map<std::string /* Animation Name */, AnimationMappingData> AnimationMapping;

            std::vector<float> BoneWeights;

            std::vector<int32_t> BoneIndices;

         private:

            std::map<std::string /* Bone Name */, size_t /* Bone index */> BoneIndexMapping;

         public:

            Collector(const aiScene* scene);

            void Collect();

         private:

            void CollectNodeHierarchy(const aiNode* pNode, MeshNode* meshNode);

            void CollectBones();

            void CollectAnimation();

            void AnimationIterateNodes(const aiAnimation* pAnimation, const aiNode* pNode, AnimationMappingData::NodeAnimationBinding_t& nodeAnimationBindings);

            void CollectVertexData();

            void VertexDataIterate(size_t meshBaseVertexIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& vertexBoneData);

            void StoreVertexBoneData(const std::vector<VertexBoneData>& vertexBoneData);
         };

         struct AnimatedMeshData
         {
            // Node hierarchy root
            MeshNode* RootNode = nullptr;

            // Bone info
            std::map<std::string /* Bone Name */, MeshBoneInfo> BoneMapping;

            std::map<std::string /* Animation Name */, AnimationMappingData> AnimationMapping;

            glm::mat4 GlobalInverseTransform;

            AnimatedMeshData(const Collector& collector)
               : RootNode(collector.meshRootNode)
               , BoneMapping(std::move(collector.BoneMapping))
               , AnimationMapping(std::move(collector.AnimationMapping))
               , GlobalInverseTransform(std::move(collector.GlobalInverseTransform))
            {
            }

            ~AnimatedMeshData()
            {
               std::cout << "Destroy me";
            }

            std::vector<glm::mat4> GetAnimatedMatrices(const std::string& animationName, const float animationTime);

         private:

            void ReadNodeHierarchy(float animationTime, const std::string& animationName, MeshNode* node, const glm::mat4& parentTransform, std::vector<glm::mat4>& finalOutput);

            glm::vec3 InterpolateScaling(float animationTime, const std::string& animationName, const std::string& nodeName);
            glm::vec3 InterpolateTranslation(float animationTime, const std::string& animationName, const std::string& nodeName);
            glm::quat InterpolateRotation(float animationTime, const std::string& animationName, const std::string& nodeName);

            size_t FindScalingIndex(const float animationTime, const std::vector<FrameScale>& scalingFrames);
            size_t FindTranslationIndex(const float animationTime, const std::vector<FrameTranslation>& translationFrames);
            size_t FindRotationIndex(const float animationTime, const std::vector<FrameRotation>& rotationFrames);
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
