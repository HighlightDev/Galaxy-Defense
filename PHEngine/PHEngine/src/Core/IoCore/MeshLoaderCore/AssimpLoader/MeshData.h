#pragma once

#include <set>
#include <map>
#include <vector>
#include <string>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/quaternion_float.hpp>

#define MAX_BONES_PER_VERT 3

namespace MeshLoader
{
   namespace Assimp
   {
      struct MeshNode
      {
         std::string Name;
         std::vector<MeshNode*> Children;
         glm::mat4 NodeTransformation;

         ~MeshNode()
         {
            for (size_t i = 0; i < Children.size(); ++i)
            {
               delete Children[i];
            }
         }
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
         float Weights[MAX_BONES_PER_VERT]{ 0.0f };

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

         std::map<std::string /* Bone Name */, MeshBoneInfo> BoneMapping;

         glm::mat4 GlobalInverseTransform;

         std::map<std::string /* Animation Name */, AnimationMappingData> AnimationMapping;

         std::vector<float> BoneWeights;

         std::vector<int32_t> BoneIndices;

         std::vector<size_t> VertexIndices;

         std::vector<float> Positions;

         std::vector<float> TextureCoordinates;

         std::vector<float> Normals;

         std::vector<float> TangentNormals;

         std::vector<float> BitangetNormals;

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

         void StoreVertexData(const aiMesh* pMesh);

         void StoreIndices(const aiMesh* pMesh);
      };

      struct MeshAttributes
      {
         std::vector<float> BoneWeights;

         std::vector<int32_t> BoneIndices;

         std::vector<size_t> VertexIndices;

         std::vector<float> Positions;

         std::vector<float> TextureCoordinates;

         std::vector<float> Normals;

         std::vector<float> TangentNormals;

         std::vector<float> BitangetNormals;

         MeshAttributes(const Collector& collector)
            : BoneWeights(std::move(collector.BoneWeights))
            , BoneIndices(std::move(collector.BoneIndices))
            , VertexIndices(std::move(collector.VertexIndices))
            , Positions(std::move(collector.Positions))
            , TextureCoordinates(std::move(collector.TextureCoordinates))
            , Normals(std::move(collector.Normals))
            , TangentNormals(std::move(collector.TangentNormals))
            , BitangetNormals(std::move(collector.BitangetNormals))
         {
         }
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
            delete RootNode;
         }

         std::vector<glm::mat4> GetAnimatedMatricesByName(const std::string& animationName, const float animationTime);

         std::vector<glm::mat4> GetAnimatedMatricesByIndex(const size_t index, const float animationTime);

      private:

         std::vector<glm::mat4> GetAnimatedMatricesFacade(const std::string& animationName, const float animationTime);

         void ReadNodeHierarchy(float animationTime, const std::string& animationName, MeshNode* node, const glm::mat4& parentTransform, std::vector<glm::mat4>& finalOutput);

         glm::vec3 InterpolateScaling(float animationTime, const std::string& animationName, const std::string& nodeName);
         glm::vec3 InterpolateTranslation(float animationTime, const std::string& animationName, const std::string& nodeName);
         glm::quat InterpolateRotation(float animationTime, const std::string& animationName, const std::string& nodeName);

         size_t FindScalingIndex(const float animationTime, const std::vector<FrameScale>& scalingFrames);
         size_t FindTranslationIndex(const float animationTime, const std::vector<FrameTranslation>& translationFrames);
         size_t FindRotationIndex(const float animationTime, const std::vector<FrameRotation>& rotationFrames);
      };
   }
}
