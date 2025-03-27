#pragma once

#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/GraphicsCore/OpenGL/Shader/UniformBuffer.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryShader.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/UniformBufferPool.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

using namespace Graphics::OpenGL;
using namespace IO;
using namespace EngineUtility;
using namespace Resources;

#undef min
#undef max

namespace EngineCore {
template<int32_t InfluenceWeightsCount>
class SkeletalMeshVertexFactory : public VertexFactoryShader {

    UniformArray u_boneMatrices;

    std::shared_ptr<UniformBuffer> m_transformMatricesUniformBuffer;
    std::shared_ptr<UniformBuffer> m_skinningMatricesUniformBuffer;

    static inline int32_t s_instanceId = 0;

    const int32_t MaxBones;
    static constexpr int32_t MaxWeightsIndices = InfluenceWeightsCount;

public:
    SkeletalMeshVertexFactory()
        : VertexFactoryShader("SkeletalMeshVertexFactory")
        , MaxBones(EngineConfigHolder::GetInstance()->GetEngineConfig().MaxSkeletBones)
    {
        InitShader(FolderManager::GetInstance()->GetShadersPath() + "vertex_factory" + SLASH + "SkeletalMeshVertexFactory.glsl");
    }

    void AccessAllUniformLocations(uint32_t shaderProgramID) override
    {
        // u_boneMatrices = GetUniformArray("bonesMatrices", MaxBones, shaderProgramID, eShaderType::VertexShader);

        m_transformMatricesUniformBuffer = UniformBufferPool::GetInstance()->GetOrAllocateResource(UniformBufferParameters{
            "SkeletalMeshVertexFactory_" + std::to_string(s_instanceId++),
            "Matrices",
            0,
            static_cast<uint32_t>(3 + MaxBones) * static_cast<uint32_t>(sizeof(glm::mat4)),
            shaderProgramID});

        m_skinningMatricesUniformBuffer = UniformBufferPool::GetInstance()->GetOrAllocateResource(UniformBufferParameters{
            "SkinningMatrices", "SkinningMatrices", 0, static_cast<uint32_t>(MaxBones) * sizeof(glm::mat4), shaderProgramID});
    }

    void SetMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
    {
        struct MatricesInternal {
            glm::mat4 worldMatrix;
            glm::mat4 viewMatrix;
            glm::mat4 projectionMatrix;
        };
        m_transformMatricesUniformBuffer->SetData(MatricesInternal{worldMatrix, viewMatrix, projectionMatrix});
    }

    void SetSkinningMatrices(const std::vector<glm::mat4>& skinningMatrices)
    {
        struct SkinningMatrices {
            glm::mat4 matrices[MaxWeightsIndices];
        };

        SkinningMatrices matrices;

        const auto matricesCount = std::min(skinningMatrices.size(), static_cast<size_t>(MaxWeightsIndices));
        for (size_t index = 0; index < matricesCount; index++) {
            matrices.matrices[index] = skinningMatrices[index];
            // u_boneMatrices.LoadUniform(index, skinningMatrices[index]);
        }

        m_skinningMatricesUniformBuffer->SetData(&matrices);
    }

    void SetShaderPredefine() override
    {
        DefineConstant<int32_t>("MaxBones", (int32_t)MaxBones);
        DefineConstant<int32_t>("MaxWeights", (int32_t)MaxWeightsIndices);
    }

    std::vector<std::shared_ptr<AttributeDataBase>> GetVertexAttributes(const int32_t shaderProgramId) override
    {
        std::vector<std::shared_ptr<AttributeDataBase>> result;
        result.reserve(7);
        result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexPosition>>(0));
        result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexNormal>>(1));
        result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexTexCoords>>(2));
        result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexTangent>>(3));
        result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexBitangent>>(4));
        result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexBlendWeights>>(5));
        result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexBlendIndex>>(6));
        return result;
    }
};
} // namespace EngineCore