#include "MeshAllocationPolicy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/BoundingBoxBuilder.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GraphicsCore/Mesh/AnimatedSkin.h"
#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/GraphicsCore/OpenGL/CompositeVertexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/IndexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/eAttribArrayIndex.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/IoCore/MeshLoaderCore/MeshResourceInfo.h"
#include "Core/IoCore/RawResource.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"

#include <gl/glew.h>

#include <vector>

using namespace Graphics::OpenGL;
using namespace Graphics::Mesh;
using namespace MeshLoader::Assimp;
using namespace EngineCore;
using namespace IO;

namespace Resources {
std::shared_ptr<Skin> MeshAllocationPolicy::AllocateMemory(const MeshPoolParameters& arg)
{
    LogInfo("MeshAllocationPolicy::AllocateMemory: ", arg.mModelPath);

    std::shared_ptr<Skin> resultSkin;
    BoundingBox3D boundingBox;

    {
        const auto vao = std::make_shared<VertexArrayObject>();

        Resource* outResource;
        const bool bResourceValid = ResourceMap::GetInstance()->TryGetResource(outResource, arg.mModelPath);

        ext_assert(bResourceValid, "Mesh resource not found: " + arg.mModelPath);

        const MeshResource* meshResource = static_cast<MeshResource*>(outResource);
        MeshResourceInfo* meshInfo = meshResource->GetMeshResourceInfo();

        MeshAttributes* meshAttributes = meshInfo->meshAttributes;
        const auto& vertexAttributes = arg.mVertexAttributes;

        const bool isAnimatedMesh = meshInfo->meshAnimatedData != nullptr;

        ext_assert(not meshAttributes->Positions.empty(), "Mesh must have position attribute");

        if (not isAnimatedMesh) {
            struct StaticMeshVertex {
                float position[3];
                float normal[3];
                float texCoords[2];
                float tangent[3];
                float bitangent[3];
            };

            const auto stride = sizeof(StaticMeshVertex);
            auto compositeVBO = new CompositeVertexBufferObject<StaticMeshVertex>(GL_ARRAY_BUFFER, stride);
            compositeVBO->AddData(
                static_cast<int32_t>(eAttribArrayIndex::VertexPosition), GL_FLOAT, 3, offsetof(StaticMeshVertex, position));
            compositeVBO->AddData(
                static_cast<int32_t>(eAttribArrayIndex::VertexNormal), GL_FLOAT, 3, offsetof(StaticMeshVertex, normal));
            compositeVBO->AddData(
                static_cast<int32_t>(eAttribArrayIndex::VertexTexCoords), GL_FLOAT, 2, offsetof(StaticMeshVertex, texCoords));
            compositeVBO->AddData(
                static_cast<int32_t>(eAttribArrayIndex::VertexTangent), GL_FLOAT, 3, offsetof(StaticMeshVertex, tangent));
            compositeVBO->AddData(
                static_cast<int32_t>(eAttribArrayIndex::VertexBitangent), GL_FLOAT, 3, offsetof(StaticMeshVertex, bitangent));

            std::vector<StaticMeshVertex> vertexData(meshAttributes->Positions.size() / 3);
            for (size_t i = 0; i < vertexData.size(); ++i) {
                std::copy_n(&meshAttributes->Positions[i * 3], 3, vertexData[i].position);
                if (meshAttributes->Normals.empty()) {
                    std::fill_n(vertexData[i].normal, 3, 0.0f);
                } else {
                    std::copy_n(&meshAttributes->Normals[i * 3], 3, vertexData[i].normal);
                }

                if (meshAttributes->TextureCoordinates.empty()) {
                    std::fill_n(vertexData[i].texCoords, 2, 0.0f);
                } else {
                    std::copy_n(&meshAttributes->TextureCoordinates[i * 2], 2, vertexData[i].texCoords);
                }

                if (meshAttributes->TangentNormals.empty()) {
                    std::fill_n(vertexData[i].tangent, 3, 0.0f);
                } else {
                    std::copy_n(&meshAttributes->TangentNormals[i * 3], 3, vertexData[i].tangent);
                }

                if (meshAttributes->BitangetNormals.empty()) {
                    std::fill_n(vertexData[i].bitangent, 3, 0.0f);
                } else {
                    std::copy_n(&meshAttributes->BitangetNormals[i * 3], 3, vertexData[i].bitangent);
                }
            }
            compositeVBO->SetRawData(std::move(vertexData));
            vao->AddVBO(compositeVBO);

        } else {

            struct AnimatedMeshVertex {
                float position[3];
                float normal[3];
                float texCoords[2];
                float tangent[3];
                float bitangent[3];
                float blendWeights[4];
                int32_t blendIndices[4];
            };

            const auto stride = sizeof(AnimatedMeshVertex);
            auto compositeVBO = new CompositeVertexBufferObject<AnimatedMeshVertex>(GL_ARRAY_BUFFER, stride);
            compositeVBO->AddData(
                static_cast<int32_t>(eAttribArrayIndex::VertexPosition), GL_FLOAT, 3, offsetof(AnimatedMeshVertex, position));
            compositeVBO->AddData(
                static_cast<int32_t>(eAttribArrayIndex::VertexNormal), GL_FLOAT, 3, offsetof(AnimatedMeshVertex, normal));
            compositeVBO->AddData(
                static_cast<int32_t>(eAttribArrayIndex::VertexTexCoords), GL_FLOAT, 2, offsetof(AnimatedMeshVertex, texCoords));
            compositeVBO->AddData(
                static_cast<int32_t>(eAttribArrayIndex::VertexTangent), GL_FLOAT, 3, offsetof(AnimatedMeshVertex, tangent));
            compositeVBO->AddData(
                static_cast<int32_t>(eAttribArrayIndex::VertexBitangent), GL_FLOAT, 3, offsetof(AnimatedMeshVertex, bitangent));
            compositeVBO->AddData(
                static_cast<int32_t>(eAttribArrayIndex::VertexBlendWeights),
                GL_FLOAT,
                4,
                offsetof(AnimatedMeshVertex, blendWeights));
            compositeVBO->AddData(
                static_cast<int32_t>(eAttribArrayIndex::VertexBlendIndex), GL_INT, 4, offsetof(AnimatedMeshVertex, blendIndices));

            std::vector<AnimatedMeshVertex> vertexData(meshAttributes->Positions.size() / 3);
            for (size_t i = 0; i < vertexData.size(); ++i) {
                std::copy_n(&meshAttributes->Positions[i * 3], 3, vertexData[i].position);
                if (meshAttributes->Normals.empty()) {
                    std::fill_n(vertexData[i].normal, 3, 0.0f);
                } else {
                    std::copy_n(&meshAttributes->Normals[i * 3], 3, vertexData[i].normal);
                }
                if (meshAttributes->TextureCoordinates.empty()) {
                    std::fill_n(vertexData[i].texCoords, 2, 0.0f);
                } else {
                    std::copy_n(&meshAttributes->TextureCoordinates[i * 2], 2, vertexData[i].texCoords);
                }
                if (meshAttributes->TangentNormals.empty()) {
                    std::fill_n(vertexData[i].tangent, 3, 0.0f);
                } else {
                    std::copy_n(&meshAttributes->TangentNormals[i * 3], 3, vertexData[i].tangent);
                }
                if (meshAttributes->BitangetNormals.empty()) {
                    std::fill_n(vertexData[i].bitangent, 3, 0.0f);
                } else {
                    std::copy_n(&meshAttributes->BitangetNormals[i * 3], 3, vertexData[i].bitangent);
                }
                if (meshAttributes->BoneWeights.empty()) {
                    std::fill_n(vertexData[i].blendWeights, 4, 0.0f);
                } else {
                    std::copy_n(&meshAttributes->BoneWeights[i * 4], 4, vertexData[i].blendWeights);
                }
                if (meshAttributes->BoneIndices.empty()) {
                    std::fill_n(vertexData[i].blendIndices, 4, 0);
                } else {
                    std::copy_n(&meshAttributes->BoneIndices[i * 4], 4, vertexData[i].blendIndices);
                }
            }
            compositeVBO->SetRawData(std::move(vertexData));
            vao->AddVBO(compositeVBO);
        }
        BoundingBoxBuilder builder;
        boundingBox = builder.Build(meshAttributes->Positions);

        if (meshAttributes->VertexIndices.size()) {
            const auto& ibo = new IndexBufferObject(meshAttributes->VertexIndices, eDataCarryFlag::INVALIDATE);
            vao->SetIBO(ibo);
        }

        ext_assert(vao->GetVBOs().size(), "VAO must have at least one VBO");
        vao->BindBuffersToVao();

        if (meshInfo->meshAnimatedData) {
            resultSkin = std::make_shared<AnimatedSkin>(
                vao, std::shared_ptr<AnimatedMeshData>(meshInfo->meshAnimatedData), boundingBox, arg.mModelPath);
            meshInfo->meshAnimatedData = nullptr; // ownership transferred to shared_ptr
        } else {
            resultSkin = std::make_shared<Skin>(vao, boundingBox, arg.mModelPath);
        }
    }

    return resultSkin;
}
void MeshAllocationPolicy::DeallocateMemory(std::shared_ptr<Skin> arg)
{
    LogInfo("MeshAllocationPolicy::DeallocateMemory: ", arg->GetMeshName());
    arg->CleanUp();
}

} // namespace Resources
