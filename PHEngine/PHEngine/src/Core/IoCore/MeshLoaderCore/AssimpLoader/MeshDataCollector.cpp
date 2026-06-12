#include "MeshDataCollector.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/AssimpToGlmConverter.h"
#include "Core/UtilityCore/EngineMath.h"

#include <algorithm>
#include <memory>
#include <thread>
#include <tuple>

using namespace EngineUtility;

namespace MeshLoader {
namespace Assimp {

void MeshDataCollector::VertexBoneData::AddBoneData(const uint32_t boneIndex, const float weight)
{
    if (!IsFilled) {
        for (uint32_t i = 0; i < MAX_BONES_PER_VERT; ++i) {
            if (EngineMath::FloatsNearEqual(Weights[i], 0.0f)) {
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
    , MeshNodeMapping()
    , meshRootNode()
    , BoneMapping()
    , GlobalInverseTransform(1)
    , AnimationMapping()
    , AnimationIndices()
    , BoneWeights()
    , BoneIndices()
    , VertexIndices()
    , Positions()
    , TextureCoordinates()
    , Normals()
    , TangentNormals()
    , BitangetNormals()
    , BoneIndexMapping()
{
}

void MeshDataCollector::Collect()
{
    if (!mScene)
        return;

    // Node structure
    {
        aiNode* rootNode = mScene->mRootNode;

        meshRootNode = std::make_shared<MeshNode>();
        meshRootNode->Name = rootNode->mName.data;
        meshRootNode->NodeTransformation = AssimpToGlmConverter::ConvertAssimpMatrix4x4ToGlmMat4(rootNode->mTransformation);
        MeshNodeMapping[meshRootNode->Name] = meshRootNode.get();

        const aiMatrix4x4 invertedGlobalTransform = rootNode->mTransformation.Inverse();

        if (invertedGlobalTransform.a1 != std::numeric_limits<float>::quiet_NaN()) {
            GlobalInverseTransform = AssimpToGlmConverter::ConvertAssimpMatrix4x4ToGlmMat4(invertedGlobalTransform);
        }

        CollectNodeHierarchy(rootNode, meshRootNode.get());
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
    uint32_t verticesCount = 0;

    for (uint32_t i = 0; i < mScene->mNumMeshes; ++i) {
        verticesCount += mScene->mMeshes[i]->mNumVertices;
    }

    Positions.reserve(verticesCount * 3);
    TextureCoordinates.reserve(verticesCount * 2);
    Normals.reserve(verticesCount * 3);
    TangentNormals.reserve(verticesCount * 3);
    BitangetNormals.reserve(verticesCount * 3);

    std::vector<VertexBoneData> vertexBoneData;
    vertexBoneData.resize(verticesCount);

    uint32_t currentMeshBaseVertexIndex = 0;

    for (uint32_t i = 0; i < mScene->mNumMeshes; ++i) {
        VertexDataIterate(currentMeshBaseVertexIndex, mScene->mMeshes[i], vertexBoneData);
        currentMeshBaseVertexIndex += mScene->mMeshes[i]->mNumVertices;
    }

    if (mScene->HasAnimations()) {
        const uint32_t boneAttribCountPerVertex = verticesCount * MAX_BONES_PER_VERT;
        BoneWeights.resize(boneAttribCountPerVertex);
        BoneIndices.resize(boneAttribCountPerVertex);

        // Store data into array
        StoreVertexBoneData(vertexBoneData);
    }
}

void MeshDataCollector::StoreIndices(const uint32_t meshBaseVertexIndex, const aiMesh* pMesh)
{
    const uint32_t lastIndexPerMesh = VertexIndices.size();
    const uint32_t countOfFaces = pMesh->mNumFaces;

    for (uint32_t faceIndex = 0; faceIndex < countOfFaces; faceIndex++) {
        const aiFace& face = pMesh->mFaces[faceIndex];
        ext_assert(face.mNumIndices == 3, "Only triangular faces are supported");
        VertexIndices.emplace_back(static_cast<uint32_t>(face.mIndices[0]) + meshBaseVertexIndex);
        VertexIndices.emplace_back(static_cast<uint32_t>(face.mIndices[1]) + meshBaseVertexIndex);
        VertexIndices.emplace_back(static_cast<uint32_t>(face.mIndices[2]) + meshBaseVertexIndex);
    }
}

void MeshDataCollector::StoreVertexData(const aiMesh* pMesh)
{
    const bool bCollectNormals = pMesh->HasNormals();
    const bool bCollectTexCoords = pMesh->HasTextureCoords(0);
    const bool bCollectTangBitang = pMesh->HasTangentsAndBitangents();

    for (uint32_t attribIndex = 0; attribIndex < pMesh->mNumVertices; ++attribIndex) {
        Positions.emplace_back(pMesh->mVertices[attribIndex].x);
        Positions.emplace_back(pMesh->mVertices[attribIndex].y);
        Positions.emplace_back(pMesh->mVertices[attribIndex].z);

        if (bCollectNormals) {
            Normals.emplace_back(pMesh->mNormals[attribIndex].x);
            Normals.emplace_back(pMesh->mNormals[attribIndex].y);
            Normals.emplace_back(pMesh->mNormals[attribIndex].z);
        }
        if (bCollectTexCoords) {
            TextureCoordinates.emplace_back(pMesh->mTextureCoords[0][attribIndex].x);
            TextureCoordinates.emplace_back(pMesh->mTextureCoords[0][attribIndex].y);
        }
        if (bCollectTangBitang) {
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
    for (uint32_t i = 0; i < vertexBoneData.size(); ++i) {
        const VertexBoneData& vertexBoneDataItem = vertexBoneData[i];

        for (uint32_t j = 0; j < MAX_BONES_PER_VERT; ++j) {
            BoneWeights[(i * MAX_BONES_PER_VERT) + j] = vertexBoneDataItem.Weights[j];
            BoneIndices[(i * MAX_BONES_PER_VERT) + j] = vertexBoneDataItem.BoneIndices[j];
        }
    }
}

void MeshDataCollector::VertexDataIterate(
    const uint32_t meshBaseVertexIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& vertexBoneData)
{
    for (uint32_t i = 0; i < pMesh->mNumBones; ++i) {

        std::string boneName(pMesh->mBones[i]->mName.data);
        uint32_t BoneIndex = BoneIndexMapping[boneName];

        for (uint32_t j = 0; j < pMesh->mBones[i]->mNumWeights; ++j) {
            uint32_t VertexID = meshBaseVertexIndex + pMesh->mBones[i]->mWeights[j].mVertexId;
            const float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
            vertexBoneData[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }

    StoreVertexData(pMesh);
    StoreIndices(meshBaseVertexIndex, pMesh);
}

void MeshDataCollector::CollectAnimation()
{
    aiNode* rootNode = mScene->mRootNode;

    for (uint32_t i = 0; i < mScene->mNumAnimations; ++i) {
        const aiAnimation* pAnimation = mScene->mAnimations[i];

        std::string animationName(pAnimation->mName.data);
        AnimationMapping[animationName].AnimationDuration = (float)pAnimation->mDuration;
        AnimationIndices.push_back(animationName);

        AnimationIterateNodes(pAnimation, rootNode, AnimationMapping[animationName].NodeAnimationBindings);
    }
}

aiNodeAnim* FindAnimationNodeByName(const aiAnimation* pAnimation, const std::string& nodeName);

void MeshDataCollector::AnimationIterateNodes(
    const aiAnimation* pAnimation, const aiNode* pNode, AnimationMappingData::NodeAnimationBinding_t& nodeAnimationBindings)
{
    std::string nodeName(pNode->mName.data);

    aiNodeAnim* pNodeAnim = FindAnimationNodeByName(pAnimation, nodeName);

    if (pNodeAnim) {
        for (uint32_t i = 0; i < pNodeAnim->mNumRotationKeys; ++i) {
            FrameRotation rotation;
            rotation.Rotation = AssimpToGlmConverter::ConvertAssimpQuatToGlmQuat(pNodeAnim->mRotationKeys[i].mValue);
            rotation.Time = (float)pNodeAnim->mRotationKeys[i].mTime;
            nodeAnimationBindings[nodeName].RotationFrames.emplace_back(std::move(rotation));
        }

        for (uint32_t i = 0; i < pNodeAnim->mNumPositionKeys; ++i) {
            FrameTranslation translation;
            translation.Translation = AssimpToGlmConverter::ConvertAssimpVec3ToGlmVec3(pNodeAnim->mPositionKeys[i].mValue);
            translation.Time = (float)pNodeAnim->mPositionKeys[i].mTime;
            nodeAnimationBindings[nodeName].TranslationFrames.emplace_back(std::move(translation));
        }

        for (uint32_t i = 0; i < pNodeAnim->mNumScalingKeys; ++i) {
            FrameScale scale;
            scale.Scale = AssimpToGlmConverter::ConvertAssimpVec3ToGlmVec3(pNodeAnim->mScalingKeys[i].mValue);
            scale.Time = (float)pNodeAnim->mScalingKeys[i].mTime;
            nodeAnimationBindings[nodeName].ScaleFrames.emplace_back(std::move(scale));
        }
    }

    for (uint32_t i = 0; i < pNode->mNumChildren; ++i) {
        aiNode* child = pNode->mChildren[i];

        if (child) {
            AnimationIterateNodes(pAnimation, child, nodeAnimationBindings);
        }
    }
}

aiNodeAnim* FindAnimationNodeByName(const aiAnimation* pAnimation, const std::string& nodeName)
{
    aiNodeAnim* result = nullptr;

    for (uint32_t i = 0; i < pAnimation->mNumChannels; ++i) {
        aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == nodeName) {
            result = pNodeAnim;
            break;
        }
    }

    return result;
}

void MeshDataCollector::CollectBones()
{
    uint32_t meshCount = mScene->mNumMeshes;

    uint32_t totalCountBones = 0;

    for (uint32_t i = 0; i < meshCount; ++i) {
        aiMesh* mesh = mScene->mMeshes[i];

        for (uint32_t j = 0; j < mesh->mNumBones; ++j) {
            aiBone* boneInfo = mesh->mBones[j];
            const std::string& boneName = std::string(boneInfo->mName.data);
            if (BoneMapping.find(boneName) == BoneMapping.end()) {
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

    for (uint32_t i = 0; i < pNode->mNumChildren; ++i) {
        aiNode* pChildNode = pNode->mChildren[i];
        auto meshChildNode = std::make_shared<MeshNode>();
        meshChildNode->Name = pChildNode->mName.data;
        meshChildNode->NodeTransformation = AssimpToGlmConverter::ConvertAssimpMatrix4x4ToGlmMat4(pChildNode->mTransformation);
        meshNode->Children.push_back(meshChildNode);
        MeshNodeMapping[meshChildNode->Name] = meshChildNode.get();

        CollectNodeHierarchy(pChildNode, meshChildNode.get());
    }
}
} // namespace Assimp
} // namespace MeshLoader
