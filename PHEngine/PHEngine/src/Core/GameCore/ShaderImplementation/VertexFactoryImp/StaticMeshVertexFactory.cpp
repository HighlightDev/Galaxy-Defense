#include "StaticMeshVertexFactory.h"

#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/UniformBufferPool.h"

using namespace IO;
using namespace Resources;
using namespace Graphics::OpenGL;

namespace EngineCore {

int32_t StaticMeshVertexFactory::s_instanceId = 0;

StaticMeshVertexFactory::StaticMeshVertexFactory()
    : VertexFactoryShader("StaticMeshVertexFactory")
{
    InitShader(FolderManager::GetInstance()->GetAbsolutePathToRes("StaticMeshVertexFactory.glsl"));
}

void StaticMeshVertexFactory::AccessAllUniformLocations(uint32_t shaderProgramID)
{
    mWorldMatrix = GetUniform("worldMatrix", shaderProgramID);
    mViewMatrix = GetUniform("viewMatrix", shaderProgramID);
    mProjectionMatrix = GetUniform("projectionMatrix", shaderProgramID);
}

void StaticMeshVertexFactory::SetMatrices(
    const glm::mat4& worldMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    mWorldMatrix.LoadUniform(worldMatrix);
    mViewMatrix.LoadUniform(viewMatrix);
    mProjectionMatrix.LoadUniform(projectionMatrix);
}

std::vector<std::shared_ptr<AttributeDataBase>> StaticMeshVertexFactory::GetVertexAttributes(const int32_t shaderProgramId)
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
} // namespace EngineCore