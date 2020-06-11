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

         AnimatedMeshData(const struct MeshDataCollector& collector);

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

