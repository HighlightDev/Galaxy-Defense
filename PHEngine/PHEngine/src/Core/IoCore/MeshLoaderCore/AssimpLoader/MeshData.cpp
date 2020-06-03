#include "MeshData.h"
#include "Core/UtilityCore/AssimpToGlmConverter.h"
#include "Core/CommonCore/Assertion.h"

#include <tuple>
#include <thread>
#include <memory>
#include <algorithm>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>

using namespace EngineUtility;

namespace MeshLoader
{
   namespace Assimp
   {

      Collector::Collector(const aiScene* scene)
         : mScene(scene)
         , GlobalInverseTransform(1)
      {
      }

      void Collector::Collect()
      {
         if (!mScene)
            return;

         // Node structure
         {
            aiNode* rootNode = mScene->mRootNode;

            meshRootNode = new MeshNode();
            meshRootNode->Name = rootNode->mName.data;
            meshRootNode->NodeTransformation = AssimpToGlmConverter::ConvertAssimpMatrix4x4ToGlmMat4(rootNode->mTransformation);
            MeshNodeMapping[meshRootNode->Name] = meshRootNode;

            const aiMatrix4x4 invertedGlobalTransform = rootNode->mTransformation.Inverse();

            if (invertedGlobalTransform.a1 != std::numeric_limits<float>::quiet_NaN())
            {
               GlobalInverseTransform = AssimpToGlmConverter::ConvertAssimpMatrix4x4ToGlmMat4(invertedGlobalTransform);
            }

            CollectNodeHierarchy(rootNode, meshRootNode);
         }

         // Bones
         {
            CollectBones();
         }

         // Animations
         {
            CollectAnimation();
         }

         // Vertex data
         {
            CollectVertexData();
         }
      }

      void Collector::CollectVertexData()
      {
         size_t verticesCount = 0;

         for (size_t i = 0; i < mScene->mNumMeshes; ++i)
         {
            verticesCount += mScene->mMeshes[i]->mNumVertices;
         }

         std::vector<VertexBoneData> vertexBoneData;
         vertexBoneData.resize(verticesCount);

         size_t currentMeshBaseVertexIndex = 0;

         for (size_t i = 0; i < mScene->mNumMeshes; ++i)
         {
            VertexDataIterate(currentMeshBaseVertexIndex, mScene->mMeshes[i], vertexBoneData);
            currentMeshBaseVertexIndex += mScene->mMeshes[i]->mNumVertices;
         }

         const size_t boneAttribCountPerVertex = verticesCount * MAX_BONES_PER_VERT;
         BoneWeights.resize(boneAttribCountPerVertex);
         BoneIndices.resize(boneAttribCountPerVertex);

         Positions.reserve(verticesCount * 3);
         TextureCoordinates.reserve(verticesCount * 2);
         Normals.reserve(verticesCount * 3);
         TangentNormals.reserve(verticesCount * 3);
         BitangetNormals.reserve(verticesCount * 3);

         // Store data into array
         StoreVertexBoneData(vertexBoneData);
      }

      void Collector::StoreIndices(const aiMesh* pMesh)
      {
         const size_t lastIndexPerMesh = VertexIndices.size();
         const size_t countOfFaces = pMesh->mNumFaces;

         for (size_t faceIndex = 0; faceIndex < countOfFaces; faceIndex++)
         {
            const aiFace& face = pMesh->mFaces[faceIndex];

            if (face.mNumIndices == 3) // triangulated face
            {
               VertexIndices.emplace_back(face.mIndices[0]);
               VertexIndices.emplace_back(face.mIndices[1]);
               VertexIndices.emplace_back(face.mIndices[2]);
            }
            else
            {
               throw std::invalid_argument("Face isn't triangulated.");
            }
         }
      }

      void Collector::StoreVertexData(const aiMesh* pMesh)
      {
         const bool bCollectNormals = pMesh->HasNormals();
         const bool bCollectTexCoords = pMesh->HasTextureCoords(0);
         const bool bCollectTangBitang = pMesh->HasTangentsAndBitangents();

         for (size_t attribIndex = 0; attribIndex < pMesh->mNumVertices; ++attribIndex)
         {
            Positions.emplace_back(pMesh->mVertices[attribIndex].x);
            Positions.emplace_back(pMesh->mVertices[attribIndex].y);
            Positions.emplace_back(pMesh->mVertices[attribIndex].z);

            if (bCollectNormals)
            {
               Normals.emplace_back(pMesh->mNormals[attribIndex].x);
               Normals.emplace_back(pMesh->mNormals[attribIndex].y);
               Normals.emplace_back(pMesh->mNormals[attribIndex].z);
            }
            if (bCollectTexCoords)
            {
               TextureCoordinates.emplace_back(pMesh->mTextureCoords[0][attribIndex].x);
               TextureCoordinates.emplace_back(pMesh->mTextureCoords[0][attribIndex].y);
            }
            if (bCollectTangBitang)
            {
               TangentNormals.emplace_back(pMesh->mTangents[attribIndex].x);
               TangentNormals.emplace_back(pMesh->mTangents[attribIndex].y);
               TangentNormals.emplace_back(pMesh->mTangents[attribIndex].z);

               BitangetNormals.emplace_back(pMesh->mBitangents[attribIndex].x);
               BitangetNormals.emplace_back(pMesh->mBitangents[attribIndex].y);
               BitangetNormals.emplace_back(pMesh->mBitangents[attribIndex].z);
            }
         }
      }

      void Collector::StoreVertexBoneData(const std::vector<VertexBoneData>& vertexBoneData)
      {
         for (size_t i = 0; i < vertexBoneData.size(); ++i)
         {
            const VertexBoneData& vertexBoneDataItem = vertexBoneData[i];

            for (size_t j = 0; j < MAX_BONES_PER_VERT; ++j)
            {
               BoneWeights[(i * MAX_BONES_PER_VERT) + j] = vertexBoneDataItem.Weights[j];
               BoneIndices[(i * MAX_BONES_PER_VERT) + j] = vertexBoneDataItem.BoneIndices[j];
            }
         }
      }

      void Collector::VertexDataIterate(size_t meshBaseVertexIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& vertexBoneData)
      {
         for (size_t i = 0; i < pMesh->mNumBones; ++i) {

            std::string boneName(pMesh->mBones[i]->mName.data);
            size_t BoneIndex = BoneIndexMapping[boneName];

            for (size_t j = 0; j < pMesh->mBones[i]->mNumWeights; ++j) {
               size_t VertexID = meshBaseVertexIndex + pMesh->mBones[i]->mWeights[j].mVertexId;
               float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
               vertexBoneData[VertexID].AddBoneData(BoneIndex, Weight);
            }
         }

         StoreVertexData(pMesh);
         StoreIndices(pMesh);
      }

      void Collector::CollectAnimation()
      {
         aiNode* rootNode = mScene->mRootNode;


         for (size_t i = 0; i < mScene->mNumAnimations; ++i)
         {
            const aiAnimation* pAnimation = mScene->mAnimations[i];

            std::string animationName(pAnimation->mName.data);
            AnimationMapping[animationName].AnimationDuration = (float)pAnimation->mDuration;

            AnimationIterateNodes(pAnimation, rootNode, AnimationMapping[animationName].NodeAnimationBindings);
         }
      }

      aiNodeAnim* FindAnimationNodeByName(const aiAnimation* pAnimation, const std::string& nodeName);

      void Collector::AnimationIterateNodes(const aiAnimation* pAnimation, const aiNode* pNode, AnimationMappingData::NodeAnimationBinding_t& nodeAnimationBindings)
      {
         std::string nodeName(pNode->mName.data);

         aiNodeAnim* pNodeAnim = FindAnimationNodeByName(pAnimation, nodeName);

         if (pNodeAnim)
         {
            for (size_t i = 0; i < pNodeAnim->mNumRotationKeys; ++i)
            {
               FrameRotation rotation;
               rotation.Rotation = AssimpToGlmConverter::ConvertAssimpQuatToGlmQuat(pNodeAnim->mRotationKeys[i].mValue);
               rotation.Time = (float)pNodeAnim->mRotationKeys[i].mTime;
               nodeAnimationBindings[nodeName].RotationFrames.emplace_back(std::move(rotation));
            }

            for (size_t i = 0; i < pNodeAnim->mNumPositionKeys; ++i)
            {
               FrameTranslation translation;
               translation.Translation = AssimpToGlmConverter::ConvertAssimpVec3ToGlmVec3(pNodeAnim->mPositionKeys[i].mValue);
               translation.Time = (float)pNodeAnim->mPositionKeys[i].mTime;
               nodeAnimationBindings[nodeName].TranslationFrames.emplace_back(std::move(translation));
            }

            for (size_t i = 0; i < pNodeAnim->mNumScalingKeys; ++i)
            {
               FrameScale scale;
               scale.Scale = AssimpToGlmConverter::ConvertAssimpVec3ToGlmVec3(pNodeAnim->mScalingKeys[i].mValue);
               scale.Time = (float)pNodeAnim->mScalingKeys[i].mTime;
               nodeAnimationBindings[nodeName].ScaleFrames.emplace_back(std::move(scale));
            }
         }

         for (size_t i = 0; i < pNode->mNumChildren; ++i)
         {
            aiNode* child = pNode->mChildren[i];

            if (child)
            {
               AnimationIterateNodes(pAnimation, child, nodeAnimationBindings);
            }
         }
      }

      aiNodeAnim* FindAnimationNodeByName(const aiAnimation* pAnimation, const std::string& nodeName)
      {
         aiNodeAnim* result = nullptr;

         for (size_t i = 0; i < pAnimation->mNumChannels; ++i)
         {
            aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

            if (std::string(pNodeAnim->mNodeName.data) == nodeName)
            {
               result = pNodeAnim;
               break;
            }
         }

         return result;
      }

      void Collector::CollectBones()
      {
         size_t meshCount = mScene->mNumMeshes;

         size_t totalCountBones = 0;

         for (size_t i = 0; i < meshCount; ++i)
         {
            aiMesh* mesh = mScene->mMeshes[i];

            for (size_t j = 0; j < mesh->mNumBones; ++j)
            {
               aiBone* boneInfo = mesh->mBones[j];
               MeshBoneInfo meshBoneInfo;
               meshBoneInfo.BoneOffset = AssimpToGlmConverter::ConvertAssimpMatrix4x4ToGlmMat4(boneInfo->mOffsetMatrix);
               const std::string& boneName = std::string(boneInfo->mName.data);
               BoneMapping[boneName] = meshBoneInfo;
               BoneIndexMapping[boneName] = totalCountBones;
               ++totalCountBones;
            }
         }
      }

      void Collector::CollectNodeHierarchy(const aiNode* pNode, MeshNode* meshNode)
      {
         if (!pNode)
            return;

         for (size_t i = 0; i < pNode->mNumChildren; ++i)
         {
            aiNode* pChildNode = pNode->mChildren[i];
            MeshNode* meshChildNode = new MeshNode();
            meshChildNode->Name = pChildNode->mName.data;
            meshChildNode->NodeTransformation = AssimpToGlmConverter::ConvertAssimpMatrix4x4ToGlmMat4(pChildNode->mTransformation);
            meshNode->Children.push_back(meshChildNode);
            MeshNodeMapping[meshChildNode->Name] = meshChildNode;

            CollectNodeHierarchy(pChildNode, meshChildNode);
         }
      }


      std::vector<glm::mat4> AnimatedMeshData::GetAnimatedMatricesByName(const std::string& animationName, const float animationTime)
      {
         return GetAnimatedMatricesFacade(animationName, animationTime);
      }

      std::vector<glm::mat4> AnimatedMeshData::GetAnimatedMatricesByIndex(const size_t index, const float animationTime)
      {
         auto it = std::next(AnimationMapping.begin(), index);
         std::string animationName = it->first;
         return GetAnimatedMatricesFacade(animationName, animationTime);
      }

      std::vector<glm::mat4> AnimatedMeshData::GetAnimatedMatricesFacade(const std::string& animationName, const float animationTime)
      {
         std::vector<glm::mat4> FinalTransformationMatrices;
         FinalTransformationMatrices.reserve(BoneMapping.size());

         AnimationMappingData& mappingData = AnimationMapping[animationName];

         float time = fmod(animationTime, mappingData.AnimationDuration);

         ReadNodeHierarchy(time, animationName, RootNode, glm::mat4(1) /* identity */, FinalTransformationMatrices);

         return FinalTransformationMatrices;
      }

      void AnimatedMeshData::ReadNodeHierarchy(float animationTime, const std::string& animationName,
         MeshNode* node, const glm::mat4& parentTransform, std::vector<glm::mat4>& finalOutput)
      {
         const std::string& nodeName = node->Name;

         glm::mat4 nodeTransformation(node->NodeTransformation);

         // 1. Apply animation transform influence
         if (AnimationMapping[animationName].NodeAnimationBindings.count(nodeName) > 0)
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
            finalOutput.emplace_back(GlobalInverseTransform * globalTransformation *  boneOffset);
         }

         for (size_t i = 0; i < node->Children.size(); ++i)
         {
            ReadNodeHierarchy(animationTime, animationName, node->Children[i], globalTransformation, finalOutput);
         }
      }

      glm::vec3 AnimatedMeshData::InterpolateScaling(float animationTime, const std::string& animationName, const std::string& nodeName)
      {
         const std::vector<FrameScale>& scalingFrames = AnimationMapping[animationName].NodeAnimationBindings[nodeName].ScaleFrames;

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

      glm::vec3 AnimatedMeshData::InterpolateTranslation(float animationTime, const std::string& animationName, const std::string& nodeName)
      {
         const std::vector<FrameTranslation>& translationFrames = AnimationMapping[animationName].NodeAnimationBindings[nodeName].TranslationFrames;

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

      glm::quat AnimatedMeshData::InterpolateRotation(float animationTime, const std::string& animationName, const std::string& nodeName)
      {
         const std::vector<FrameRotation>& rotationFrames = AnimationMapping[animationName].NodeAnimationBindings[nodeName].RotationFrames;

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

      size_t AnimatedMeshData::FindScalingIndex(const float animationTime, const std::vector<FrameScale>& scalingFrames)
      {
         for (size_t i = 0; i < scalingFrames.size() - 1; ++i) {
            if (animationTime < scalingFrames[i + 1].Time) {
               return i;
            }
         }

         assert(0);
         return 0;
      }

      size_t AnimatedMeshData::FindTranslationIndex(const float animationTime, const std::vector<FrameTranslation>& translationFrames)
      {
         for (size_t i = 0; i < translationFrames.size() - 1; ++i) {
            if (animationTime < translationFrames[i + 1].Time) {
               return i;
            }
         }

         assert(0);
         return 0;
      }

      size_t AnimatedMeshData::FindRotationIndex(const float animationTime, const std::vector<FrameRotation>& rotationFrames)
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
