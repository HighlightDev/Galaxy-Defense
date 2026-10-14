#pragma once

#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/GraphicsCore/OpenGL/Shader/UniformBuffer.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryShader.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/UniformBufferPool.h"

using namespace Graphics::OpenGL;
using namespace IO;
using namespace Resources;

namespace EngineCore {
class GpuParticleVertexFactory : public VertexFactoryShader {

    std::shared_ptr<UniformBuffer> u_transformMatricesBuffer;

    static inline int32_t s_instanceId = 0;

public:
    explicit GpuParticleVertexFactory()
        : VertexFactoryShader("GpuParticleVertexFactory")
    {
        InitShader(FolderManager::GetInstance()->GetAbsolutePathToRes("GpuParticleVertexFactory.glsl"));
    }

    void AccessAllUniformLocations(uint32_t shaderProgramID) override
    {
        u_transformMatricesBuffer = UniformBufferPool::GetInstance()->GetOrAllocateResource(UniformBufferParameters{
            "GpuParticleVertexFactory_" + std::to_string(s_instanceId++), "Matrices", 0, sizeof(glm::mat4) * 3, shaderProgramID});
    }

    void SetMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
    {
        struct InternalMatrices {
            glm::mat4 worldMatrix;
            glm::mat4 viewMatrix;
            glm::mat4 projectionMatrix;
        };

        InternalMatrices matrices{worldMatrix, viewMatrix, projectionMatrix};
        u_transformMatricesBuffer->SetData(matrices);
    }

    std::vector<std::shared_ptr<AttributeDataBase>> GetVertexAttributes(const int32_t shaderProgramId) override
    {
        std::vector<std::shared_ptr<AttributeDataBase>> result;
        result.reserve(1);
        result.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexPosition>>(0));
        return result;
    }
};
} // namespace EngineCore