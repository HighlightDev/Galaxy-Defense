#pragma once

#include "Core/IoCore/MeshLoaderCore/MeshNode.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/MeshDataCollector.h"
#include "Core/IoCore/MeshLoaderCore/AnimationSequenceData.h"

using namespace MeshLoader;
using namespace MeshLoader::Assimp;

namespace EngineCore
{
   struct Transform;
}

namespace Graphics
{
   namespace Mesh
   {
      using EngineCore::Transform;
      struct AnimatedMeshData
      {
         struct BoneData
         {
            glm::vec3 Scale;
            glm::quat Rotation;
            glm::vec3 Translation;
         };

         // Node hierarchy root
         MeshNode* RootNode = nullptr;

         // Bone info
         std::map<std::string /* Bone Name */, MeshBoneInfo> BoneMapping;

         std::map<std::string /* Animation Name */, AnimationMappingData> AnimationMapping;
         std::vector<std::string /*Animation Name*/> AnimationIndices;

         glm::mat4 GlobalInverseTransform;

         AnimatedMeshData(const struct MeshDataCollector& collector);

         ~AnimatedMeshData()
         {
            delete RootNode;
         }

         std::vector<glm::mat4> GetAnimatedMatrices(const std::string& animationName, const float animationTime) const;

         std::vector<glm::mat4> GetAnimatedMatricesWithBlendedBoneData(const std::map<std::string, BoneData>& blendedBoneData);

         std::map<std::string, BoneData> GetBoneMappingForBlendedAnimation(const std::string& srcAnimationName, const std::string& dstAnimationName,
            const float srcAnimationTime, const float dstAnimationTime, const float blendFactor);

      private:

         void GetBlendedBoneDataNodeHierarchy(const float srcTime, const float dstTime, const std::string& srcAnimName, const std::string& dstAnimName,
            const float blendFactor, MeshNode* node, std::map<std::string, BoneData>& blendedBoneData) const;

         void ReadNodeHierarchy(float animationTime, const std::string& animationName, MeshNode* node, const glm::mat4& parentTransform, std::vector<glm::mat4>& finalOutput) const;
         void ReadNodeHierarchyWithBlendedBoneData( MeshNode* node, const std::map<std::string, BoneData>& blendedBoneData, const glm::mat4& parentTransform, std::vector<glm::mat4>& finalOutput) const;

         glm::vec3 InterpolateScaling(float animationTime, const std::string& animationName, const std::string& nodeName) const;
         glm::vec3 InterpolateTranslation(float animationTime, const std::string& animationName, const std::string& nodeName) const;
         glm::quat InterpolateRotation(float animationTime, const std::string& animationName, const std::string& nodeName) const;

         size_t FindScalingIndex(const float animationTime, const std::vector<FrameScale>& scalingFrames) const;
         size_t FindTranslationIndex(const float animationTime, const std::vector<FrameTranslation>& translationFrames) const;
         size_t FindRotationIndex(const float animationTime, const std::vector<FrameRotation>& rotationFrames) const;

         static BoneData BlendBoneData(const BoneData& src, const BoneData& dst, const float blendFactor);
      };

   }
}

