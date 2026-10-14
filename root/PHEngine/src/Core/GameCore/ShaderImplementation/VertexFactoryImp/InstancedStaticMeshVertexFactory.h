#pragma once

#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryShader.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

using namespace Graphics::OpenGL;
using namespace IO;
using namespace EngineUtility;

namespace EngineCore {
class InstancedStaticMeshVertexFactory : public VertexFactoryShader {

    UniformArray u_worldMatrices;
    Uniform u_viewMatrix;
    Uniform u_projectionMatrix;

    static inline int32_t s_instanceId = 0;

public:
    explicit InstancedStaticMeshVertexFactory()
        : VertexFactoryShader("InstancedStaticMeshVertexFactory")
    {
        InitShader(FolderManager::GetInstance()->GetAbsolutePathToRes("InstancedStaticMeshVertexFactory.glsl"));
    }

    void AccessAllUniformLocations(uint32_t shaderProgramID) override
    {
        const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
        u_worldMatrices
            = GetUniformArray("worldMatrices", cfg.MaxStaticMeshInstancesPerBatch, shaderProgramID, eShaderType::VertexShader);
        u_viewMatrix = GetUniform("viewMatrix", shaderProgramID);
        u_projectionMatrix = GetUniform("projectionMatrix", shaderProgramID);
    }

    void SetMatrices(const std::vector<glm::mat4>& worldMatrices, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
    {
        for (int i = 0; i < worldMatrices.size(); ++i) {
            u_worldMatrices.LoadUniform(i, worldMatrices[i]);
        }
        u_viewMatrix.LoadUniform(viewMatrix);
        u_projectionMatrix.LoadUniform(projectionMatrix);
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