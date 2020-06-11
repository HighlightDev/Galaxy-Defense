#include "MeshDataCollector.h"
#include "Core/UtilityCore/AssimpToGlmConverter.h"
#include "Core/CommonCore/Assertion.h"

#include <tuple>
#include <thread>
#include <memory>
#include <algorithm>

using namespace EngineUtility;

namespace MeshLoader
{
   namespace Assimp
   {

      void MeshDataCollector::VertexBoneData::AddBoneData(size_t boneIndex, float weight)
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

      MeshDataCollector::MeshDataCollector(const aiScene* scene)
         : mScene(scene)
         , GlobalInverseTransform(1)
      {
      }

      void MeshDataCollector::Collect()
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

      void MeshDataCollector::CollectVertexData()
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

      void MeshDataCollector::StoreIndices(size_t meshBaseVertexIndex, const aiMesh* pMesh)
      {
         const size_t lastIndexPerMesh = VertexIndices.size();
         const size_t countOfFaces = pMesh->mNumFaces;

         for (size_t faceIndex = 0; faceIndex < countOfFaces; faceIndex++)
         {
            const aiFace& face = pMesh->mFaces[faceIndex];
            assert(face.mNumIndices == 3);
            VertexIndices.emplace_back(face.mIndices[0] + meshBaseVertexIndex);
            VertexIndices.emplace_back(face.mIndices[1] + meshBaseVertexIndex);
            VertexIndices.emplace_back(face.mIndices[2] + meshBaseVertexIndex);
         }
      }

      void MeshDataCollector::StoreVertexData(const aiMesh* pMesh)
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

      void MeshDataCollector::StoreVertexBoneData(const std::vector<VertexBoneData>& vertexBoneData)
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

      void MeshDataCollector::VertexDataIterate(size_t meshBaseVertexIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& vertexBoneData)
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
         StoreIndices(meshBaseVertexIndex, pMesh);
      }

      void MeshDataCollector::CollectAnimation()
      {
         aiNode* rootNode = mScene->mRootNode;

         for (size_t i = 0; i < mScene->mNumAnimations; ++i)
         {
            const aiAnimation* pAnimation = mScene->mAnimations[i];

            std::string animationName(pAnimation->mName.data);
            AnimationMapping[animationName].AnimationDuration = (float)pAnimation->mDuration;
            AnimationIndices.push_back(animationName);

            AnimationIterateNodes(pAnimation, rootNode, AnimationMapping[animationName].NodeAnimationBindings);
         }
      }

      aiNodeAnim* FindAnimationNodeByName(const aiAnimation* pAnimation, const std::string& nodeName);

      void MeshDataCollector::AnimationIterateNodes(const aiAnimation* pAnimation, const aiNode* pNode, AnimationMappingData::NodeAnimationBinding_t& nodeAnimationBindings)
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

      void MeshDataCollector::CollectBones()
      {
         size_t meshCount = mScene->mNumMeshes;

         size_t totalCountBones = 0;

         for (size_t i = 0; i < meshCount; ++i)
         {
            aiMesh* mesh = mScene->mMeshes[i];

            for (size_t j = 0; j < mesh->mNumBones; ++j)
            {
               aiBone* boneInfo = mesh->mBones[j];
               const std::string& boneName = std::string(boneInfo->mName.data);
               if (BoneMapping.find(boneName) == BoneMapping.end())
               {
                  MeshBoneInfo meshBoneInfo;
                  meshBoneInfo.BoneOffset = AssimpToGlmConverter::ConvertAssimpMatrix4x4ToGlmMat4(boneInfo->mOffsetMatrix);

                  BoneMapping[boneName] = meshBoneInfo;
                  BoneIndexMapping[boneName] = totalCountBones;
                  ++totalCountBones;
               }
            }
         }
      }

      void MeshDataCollector::CollectNodeHierarchy(const aiNode* pNode, MeshNode* meshNode)
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
   }
}
