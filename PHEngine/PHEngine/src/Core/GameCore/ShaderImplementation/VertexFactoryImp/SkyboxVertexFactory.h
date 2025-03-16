#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/UniformBuffer.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryShader.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/UniformBufferPool.h"

using namespace Graphics::OpenGL;
using namespace IO;
using namespace Resources;

namespace EngineCore {
class SkyboxVertexFactory : public VertexFactoryShader {

    std::shared_ptr<UniformBuffer> u_transformMatricesBuffer;

public:
    SkyboxVertexFactory()
        : VertexFactoryShader("SkyboxVertexFactory")
    {
        InitShader(FolderManager::GetInstance()->GetShadersPath() + "vertex_factory" + SLASH + "SkyboxVertexFactory.glsl");
    }

    void AccessAllUniformLocations(uint32_t shaderProgramID) override
    {
        u_transformMatricesBuffer = UniformBufferPool::GetInstance()->GetOrAllocateResource(
            UniformBufferParameters{"SkyboxVertexFactory", "Matrices", 0, sizeof(glm::mat4) * 3 * 2, shaderProgramID});
    }

    void SetMatrices(const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
    {
        struct MatricesInternal {
            glm::mat4 worldMatrix;
            glm::mat4 viewMatrix;
            glm::mat4 projectionMatrix;
        };

        u_transformMatricesBuffer->SetData(MatricesInternal{worldMatrix, viewMatrix, projectionMatrix});
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