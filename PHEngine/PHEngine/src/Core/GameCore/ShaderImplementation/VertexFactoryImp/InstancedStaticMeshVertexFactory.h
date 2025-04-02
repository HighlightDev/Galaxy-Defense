#pragma once

#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/GraphicsCore/OpenGL/Shader/UniformBuffer.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryShader.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/UniformBufferPool.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

using namespace Graphics::OpenGL;
using namespace IO;
using namespace Resources;
using namespace EngineUtility;

namespace EngineCore {
class InstancedStaticMeshVertexFactory : public VertexFactoryShader {

    UniformArray u_worldMatrices;
    Uniform u_viewMatrix;
    Uniform u_projectionMatrix;

    static inline int32_t s_instanceId = 0;

    std::shared_ptr<UniformBuffer> u_transformMatricesBuffer;

public:
    explicit InstancedStaticMeshVertexFactory()
        : VertexFactoryShader("InstancedStaticMeshVertexFactory")
    {
        InitShader(
            FolderManager::GetInstance()->GetShadersPath() + "vertex_factory" + SLASH + "InstancedStaticMeshVertexFactory.glsl");
    }

    void AccessAllUniformLocations(uint32_t shaderProgramID) override
    {
        const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
        u_transformMatricesBuffer = UniformBufferPool::GetInstance()->GetOrAllocateResource(UniformBufferParameters{
            "InstancedStaticMeshVertexFactory_" + std::to_string(s_instanceId++),
            "Matrices",
            0,
            static_cast<uint32_t>(sizeof(glm::mat4)) * static_cast<uint32_t>((3 + cfg.MaxStaticMeshInstancesPerBatch)),
            shaderProgramID});
    }

    void SetMatrices(const std::vector<glm::mat4>& worldMatrices, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
    {
        std::vector<glm::mat4> shaderMatrices;
        shaderMatrices.reserve(worldMatrices.size() + 2);
        shaderMatrices.emplace_back(viewMatrix); // view and projection matrices go first
        shaderMatrices.emplace_back(projectionMatrix);
        shaderMatrices.insert(shaderMatrices.end(), worldMatrices.begin(), worldMatrices.end());
        u_transformMatricesBuffer->SetData(shaderMatrices);
    }

    void SetShaderPredefine() override
    {
        const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
        DefineConstant<int32_t>("MAX_STATIC_MESH_INSTANCES_PER_BATCH", cfg.MaxStaticMeshInstancesPerBatch);
    }

    std::vector<std::shared_ptr<AttributeDataBase>> GetVertexAttributes(const int32_t shaderProgramId) override
    {
        std::vector<std::shared_ptr<AttributeDataBase>> result;
        result.reserve(5);
        result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexPosition>>(0));
        result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexNormal>>(1));
        result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexTexCoords>>(2));
        result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexTangent>>(3));
        result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexBitangent>>(4));
        return result;
    }
};
} // namespace EngineCore