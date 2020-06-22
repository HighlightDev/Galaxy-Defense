#include "AnimatedMeshData.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

namespace Graphics
{
   namespace Mesh
   {

      AnimatedMeshData::AnimatedMeshData(const MeshDataCollector& collector)
         : RootNode(collector.meshRootNode)
         , BoneMapping(std::move(collector.BoneMapping))
         , AnimationMapping(std::move(collector.AnimationMapping))
         , AnimationIndices(std::move(collector.AnimationIndices))
         , GlobalInverseTransform(std::move(collector.GlobalInverseTransform))
      {
      }

      std::vector<glm::mat4> AnimatedMeshData::GetAnimatedMatrices(const std::string& animationName, const float animationTime) const
      {
         std::vector<glm::mat4> FinalTransformationMatrices;
         FinalTransformationMatrices.reserve(BoneMapping.size());

         const AnimationMappingData& mappingData = AnimationMapping.at(animationName);

         float time = fmod(animationTime, mappingData.AnimationDuration);

         ReadNodeHierarchy(time, animationName, RootNode, glm::mat4(1) /* identity */, FinalTransformationMatrices);

         return FinalTransformationMatrices;
      }

      std::map<std::string, AnimatedMeshData::AnimationBoneData_t> AnimatedMeshData::GetAnimationBoneMapping(const std::string& animationName, const float animationTime)
      {
         std::map<std::string, AnimatedMeshData::AnimationBoneData_t> animationBoneData;

         const AnimationMappingData& mappingData = AnimationMapping.at(animationName);

         float time = fmod(animationTime, mappingData.AnimationDuration);

         GetBoneDataNodeHierarchy(time, animationName, RootNode, animationBoneData);

         return animationBoneData;
      }

      void AnimatedMeshData::GetBoneDataNodeHierarchy(float animationTime, const std::string& animationName,
         MeshNode* node, std::map<std::string, AnimatedMeshData::AnimationBoneData_t>& animationBoneData) const
      {
         const std::string& nodeName = node->Name;

         if (AnimationMapping.at(animationName).NodeAnimationBindings.count(nodeName) > 0)
         {
            const glm::vec3& scale = InterpolateScaling(animationTime, animationName, nodeName);
            const glm::vec3& translation = InterpolateTranslation(animationTime, animationName, nodeName);
            const glm::quat& rotation = InterpolateRotation(animationTime, animationName, nodeName);

            animationBoneData[nodeName] = std::make_tuple(scale, rotation, translation);
         }

         for (size_t i = 0; i < node->Children.size(); ++i)
         {
            GetBoneDataNodeHierarchy(animationTime, animationName, node->Children[i], animationBoneData);
         }
      }

      void AnimatedMeshData::ReadNodeHierarchy(float animationTime, const std::string& animationName,
         MeshNode* node, const glm::mat4& parentTransform, std::vector<glm::mat4>& finalOutput) const
      {
         const std::string& nodeName = node->Name;

         glm::mat4 nodeTransformation(1);
         //node->NodeTransformation);

      // 1. Apply animation transform influence
         if (AnimationMapping.at(animationName).NodeAnimationBindings.count(nodeName) > 0)
         {
            //const glm::vec3& scale = InterpolateScaling(animationTime, animationName, nodeName);
            const glm::vec3& translation = InterpolateTranslation(animationTime, animationName, nodeName);
            const glm::quat& rotation = InterpolateRotation(animationTime, animationName, nodeName);

            glm::mat4 identityMatrix(1);
            glm::mat4 translationMatrix = glm::translate(identityMatrix, translation);
            //glm::mat4 scaleMatrix = glm::scale(identityMatrix, scale);
            glm::mat4 rotationMatrix = glm::toMat4(rotation);

            nodeTransformation = translationMatrix * rotationMatrix/* * scaleMatrix*/;
         }

         // 2. Apply parent transform influence
         glm::mat4 globalTransformation = parentTransform * nodeTransformation;

         // 3. Apply bone transform influence
         if (const auto& cit = BoneMapping.find(nodeName); cit != BoneMapping.end())
         {
            const glm::mat4& boneOffset = cit->second.BoneOffset;
            finalOutput.emplace_back(/*GlobalInverseTransform * */globalTransformation *  boneOffset);
         }

         for (size_t i = 0; i < node->Children.size(); ++i)
         {
            ReadNodeHierarchy(animationTime, animationName, node->Children[i], globalTransformation, finalOutput);
         }
      }

      glm::vec3 AnimatedMeshData::InterpolateScaling(float animationTime, const std::string& animationName, const std::string& nodeName) const
      {
         const std::vector<FrameScale>& scalingFrames = AnimationMapping.at(animationName).NodeAnimationBindings.at(nodeName).ScaleFrames;

         assert(scalingFrames.size() > 0);

         // we need at least two values to interpolate...
         if (scalingFrames.size() == 1)
         {
            return scalingFrames[0].Scale;
         }

         size_t scalingIndex = FindScalingIndex(animationTime, scalingFrames);
         size_t nextScalingIndex = scalingIndex + 1;

         assert(nextScalingIndex < scalingFrames.size());

         const float DeltaTime = scalingFrames[nextScalingIndex].Time - scalingFrames[scalingIndex].Time;
         const float Factor = (animationTime - scalingFrames[scalingIndex].Time) / DeltaTime;
         return glm::lerp(scalingFrames[scalingIndex].Scale, scalingFrames[nextScalingIndex].Scale, Factor);
      }

      glm::vec3 AnimatedMeshData::InterpolateTranslation(float animationTime, const std::string& animationName, const std::string& nodeName) const
      {
         const std::vector<FrameTranslation>& translationFrames = AnimationMapping.at(animationName).NodeAnimationBindings.at(nodeName).TranslationFrames;

         assert(translationFrames.size() > 0);

         // we need at least two values to interpolate...
         if (translationFrames.size() == 1)
         {
            return translationFrames[0].Translation;
         }

         size_t translationIndex = FindTranslationIndex(animationTime, translationFrames);
         size_t nextTranslationIndex = translationIndex + 1;

         assert(nextTranslationIndex < translationFrames.size());

         const float DeltaTime = translationFrames[nextTranslationIndex].Time - translationFrames[translationIndex].Time;
         const float Factor = (animationTime - translationFrames[translationIndex].Time) / DeltaTime;
         return glm::lerp(translationFrames[translationIndex].Translation, translationFrames[nextTranslationIndex].Translation, Factor);
      }

      glm::quat AnimatedMeshData::InterpolateRotation(float animationTime, const std::string& animationName, const std::string& nodeName) const
      {
         const std::vector<FrameRotation>& rotationFrames = AnimationMapping.at(animationName).NodeAnimationBindings.at(nodeName).RotationFrames;

         assert(rotationFrames.size() > 0);

         // we need at least two values to interpolate...
         if (rotationFrames.size() == 1)
         {
            return rotationFrames[0].Rotation;
         }

         size_t rotationIndex = FindRotationIndex(animationTime, rotationFrames);
         size_t nextRotationIndex = rotationIndex + 1;

         assert(nextRotationIndex < rotationFrames.size());

         const float DeltaTime = rotationFrames[nextRotationIndex].Time - rotationFrames[rotationIndex].Time;
         const float Factor = (animationTime - rotationFrames[rotationIndex].Time) / DeltaTime;
         return glm::slerp(rotationFrames[rotationIndex].Rotation, rotationFrames[nextRotationIndex].Rotation, Factor);
      }

      size_t AnimatedMeshData::FindScalingIndex(const float animationTime, const std::vector<FrameScale>& scalingFrames) const
      {
         for (size_t i = 0; i < scalingFrames.size() - 1; ++i) {
            if (animationTime < scalingFrames[i + 1].Time) {
               return i;
            }
         }

         assert(0);
         return 0;
      }

      size_t AnimatedMeshData::FindTranslationIndex(const float animationTime, const std::vector<FrameTranslation>& translationFrames) const
      {
         for (size_t i = 0; i < translationFrames.size() - 1; ++i) {
            if (animationTime < translationFrames[i + 1].Time) {
               return i;
            }
         }

         assert(0);
         return 0;
      }

      size_t AnimatedMeshData::FindRotationIndex(const float animationTime, const std::vector<FrameRotation>& rotationFrames) const
      {
         for (size_t i = 0; i < rotationFrames.size() - 1; ++i) {
            if (animationTime < rotationFrames[i + 1].Time) {
               return i;
            }
         }

         assert(0);
         return 0;
      }
   }
}