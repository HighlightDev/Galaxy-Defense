#pragma once

#include "Core/IoCore/MeshLoaderCore/AnimationSequenceData.h"
#include "Core/IoCore/MeshLoaderCore/MeshBoneInfo.h"
#include "Core/IoCore/MeshLoaderCore/MeshNode.h"

#include <assimp/mesh.h>
#include <assimp/scene.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#define MAX_BONES_PER_VERT 4

using namespace MeshLoader;

namespace MeshLoader {
namespace Assimp {

struct MeshDataCollector {
private:
    struct VertexBoneData {
        size_t BoneIndices[MAX_BONES_PER_VERT]{0};
        float Weights[MAX_BONES_PER_VERT]{0.0f};

        bool IsFilled = false;

        void AddBoneData(size_t boneIndex, float weight);
    };

public:
    const aiScene* mScene;

    std::map<std::string /* Node Name */, MeshNode*> MeshNodeMapping;
    MeshNode* meshRootNode = nullptr;

    std::map<std::string /* Bone Name */, MeshBoneInfo> BoneMapping;

    glm::mat4 GlobalInverseTransform;

    std::map<std::string /* Animation Name */, AnimationMappingData> AnimationMapping;
    std::vector<std::string /*Animation Name*/> AnimationIndices;

    std::vector<float> BoneWeights;

    std::vector<int32_t> BoneIndices;

    std::vector<uint32_t> VertexIndices;

    std::vector<float> Positions;

    std::vector<float> TextureCoordinates;

    std::vector<float> Normals;

    std::vector<float> TangentNormals;

    std::vector<float> BitangetNormals;

    std::map<std::string /* Bone Name */, uint32_t /* Bone index */> BoneIndexMapping;

public:
    MeshDataCollector(const aiScene* scene);

    void Collect();

private:
    void CollectNodeHierarchy(const aiNode* pNode, MeshNode* meshNode);

    void CollectBones();

    void CollectAnimation();

    void AnimationIterateNodes(
        const aiAnimation* pAnimation, const aiNode* pNode, AnimationMappingData::NodeAnimationBinding_t& nodeAnimationBindings);

    void CollectVertexData();

    void VertexDataIterate(size_t meshBaseVertexIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& vertexBoneData);

    void StoreVertexBoneData(const std::vector<VertexBoneData>& vertexBoneData);

    void StoreVertexData(const aiMesh* pMesh);

    void StoreIndices(size_t meshBaseVertexIndex, const aiMesh* pMesh);
};

} // namespace Assimp
} // namespace MeshLoader
