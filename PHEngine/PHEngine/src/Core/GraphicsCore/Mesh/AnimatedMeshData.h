#pragma once

#include "Core/IoCore/MeshLoaderCore/MeshNode.h"
#include "Core/IoCore/MeshLoaderCore/AssimpLoader/MeshDataCollector.h"
#include "Core/IoCore/MeshLoaderCore/AnimationSequenceData.h"

using namespace MeshLoader;
using namespace MeshLoader::Assimp;

namespace Graphics
{
   namespace Mesh
   {

      struct AnimatedMeshData
      {
         // Node hierarchy root
         MeshNode* RootNode = nullptr;

         // Bone info
         std::map<std::string /* Bone Name */, MeshBoneInfo> BoneMapping;

         std::map<std::string /* Animation Name */, AnimationMappingData> AnimationMapping;
         std::vector<std::string /*Animation Name*/> AnimationIndices;

         glm::mat4 GlobalInverseTransform;

      public:

         AnimatedMeshData(const struct MeshDataCollector& collector);

         ~AnimatedMeshData()
         {
            delete RootNode;
         }

         using AnimationBoneData_t = std::tuple <glm::vec3 /*scale*/, glm::quat/*rotation*/, glm::vec3/*translation*/>;

         std::vector<glm::mat4> GetAnimatedMatrices(const std::string& animationName, const float animationTime) const;

         std::map<AnimationBoneData_t> GetAnimationBoneMapping(const std::string& animationName, const float animationTime);

      private:

         void GetBoneDataNodeHierarchy(float animationTime, const std::string& animationName, MeshNode* node, std::map<AnimationBoneData_t>& boneData) const;

         void ReadNodeHierarchy(float animationTime, const std::string& animationName, MeshNode* node, const glm::mat4& parentTransform, std::vector<glm::mat4>& finalOutput) const;

         glm::vec3 InterpolateScaling(float animationTime, const std::string& animationName, const std::string& nodeName) const;
         glm::vec3 InterpolateTranslation(float animationTime, const std::string& animationName, const std::string& nodeName) const;
         glm::quat InterpolateRotation(float animationTime, const std::string& animationName, const std::string& nodeName) const;

         size_t FindScalingIndex(const float animationTime, const std::vector<FrameScale>& scalingFrames) const;
         size_t FindTranslationIndex(const float animationTime, const std::vector<FrameTranslation>& translationFrames) const;
         size_t FindRotationIndex(const float animationTime, const std::vector<FrameRotation>& rotationFrames) const;
      };

   }
}

