#include "ElectricBeamSceneProxy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Components/ProceduralBeamGeometry.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/SceneViewInfo/SceneView.h"
#include "Core/ResourceManagerCore/Pool/RuntimeGeneratedMeshPool.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace Resources;
using namespace Graphics;
using namespace Graphics::Mesh;
using namespace Graphics::OpenGL;
using namespace Graphics::Renderer;

namespace Graphics {
namespace Proxy {

ElectricBeamSceneProxy::ElectricBeamSceneProxy(const ElectricBeamComponent* component)
    : PrimitiveSceneProxy(component, component->GetRenderData().mMaterialProxy)
    , mBeamMeshPoolParams(component->GetRuntimeGeneratedMeshPoolParameters())
    , mBeamCount(component->GetBeamCount())
    , mRadialSegments(component->GetRadialSegments())
    , mLengthSegments(component->GetLengthSegments())
{
}

void ElectricBeamSceneProxy::PostConstructorInitialize()
{
    static constexpr uint64_t functionId = Hash64_CT("ElectricBeamSceneProxy::PostConstructorInitialize");

    const ShaderParams shaderParams(
        "ElectricBeam_BaseShader",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "runtimeGeneratedMeshVS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "forwardNoLitFS.glsl");

    m_shader = CreateMaterialShader<StaticMeshVertexFactory, SimpleShader>(
        "StaticMeshVertexFactory_SimpleShader_" + mMaterialProxy->MaterialName, shaderParams, mMaterialProxy);

    const ShaderParams planarReflectionParams(
        "PlanarReflectionShader",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "planarReflectionVS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "forwardNoLitFS.glsl");

    m_planarReflectionShader = CreateMaterialShader<StaticMeshVertexFactory, CapturePlanarReflectionShader>(
        "StaticMeshVertexFactory_CapturePlanarReflectionShader_" + mMaterialProxy->MaterialName,
        planarReflectionParams,
        mMaterialProxy);

    CreateBeamSkins();
}

void ElectricBeamSceneProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    if (bUpdateBeamGeometry) {
        UpdateBeamGeometry();
        bUpdateBeamGeometry = false;
    }

    const auto& shader = GetShader();

    const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(shader->GetShaderName());
    if (needToRebindShader) {
        shader->ExecuteShader();
    }

    shader->GetMaterialShader()->LoadUniformValues(mMaterialProxy, activeBindedState);
    shader->GetVertexFactoryShader()->SetMatrices(m_worldMatrix, viewMatrix, projectionMatrix);

    // Render each beam
    if (mBeamSkin) {
        const auto& buffer = mBeamSkin->GetBuffer();
        buffer->RenderVAO(GL_TRIANGLES);
    }
}

bool ElectricBeamSceneProxy::IsDeferred() const
{
    return false;
}

void ElectricBeamSceneProxy::SetMeshData(const std::vector<std::tuple<std::vector<BeamVertex>, std::vector<uint32_t>>>& meshData)
{
    mMeshData = meshData;
    bUpdateBeamGeometry = true;
}

bool ElectricBeamSceneProxy::IsFrustumCullTestNeeded() const
{
    return false; // Electric beams are typically thin and might be culled incorrectly
}

std::shared_ptr<ElectricBeamSceneProxy::ShaderType> ElectricBeamSceneProxy::GetShader() const
{
    return std::static_pointer_cast<ElectricBeamSceneProxy::ShaderType>(m_shader);
}

RenderInfo ElectricBeamSceneProxy::GetRenderInfo() const
{
    RenderInfo renderInfo;
    renderInfo.mShaderName = GetShader()->GetShaderName();
    return renderInfo;
}

void ElectricBeamSceneProxy::UpdateBeamGeometry()
{
    if (mMeshData.empty()) {
        return;
    }

    std::vector<float> vertexData;
    std::vector<float> texCoordsData;
    std::vector<float> normalData;
    std::vector<uint32_t> totalIndices;
    vertexData.reserve(mBeamMeshPoolParams.mMaxVerticesCount);
    texCoordsData.reserve(mBeamMeshPoolParams.mMaxVerticesCount);
    normalData.reserve(mBeamMeshPoolParams.mMaxVerticesCount);
    totalIndices.reserve(mBeamMeshPoolParams.mMaxIndicesCount);
    const uint32_t verticesPerBeam = mBeamMeshPoolParams.mMaxVerticesCount / mBeamCount;

    for (uint32_t i = 0; i < mBeamCount; ++i) {
        const auto& vertices = std::get<0>(mMeshData[i]);
        const auto& indices = std::get<1>(mMeshData[i]);

        if (!vertices.empty() && !indices.empty()) {
            for (const auto& vertex : vertices) {
                vertexData.push_back(vertex.Position.x);
                vertexData.push_back(vertex.Position.y);
                vertexData.push_back(vertex.Position.z);
                normalData.push_back(vertex.Normal.x);
                normalData.push_back(vertex.Normal.y);
                normalData.push_back(vertex.Normal.z);
                texCoordsData.push_back(vertex.TexCoord.x);
                texCoordsData.push_back(vertex.TexCoord.y);
            }
        }

        for (const auto& index : indices) {
            totalIndices.push_back(index + (i * verticesPerBeam));
        }
    }
    auto& buffer = mBeamSkin->GetBuffer();
    const auto& positionVBO = buffer->GetVboByAttribArrayIndexName("VertexPosition");
    const auto& normalVBO = buffer->GetVboByAttribArrayIndexName("VertexNormal");
    const auto& texCoordVBO = buffer->GetVboByAttribArrayIndexName("VertexTexCoords");
    const auto& ibo = buffer->GetIBO();

    positionVBO->BufferSubData(0, vertexData.size() * sizeof(float), vertexData.data());
    normalVBO->BufferSubData(0, normalData.size() * sizeof(float), normalData.data());
    texCoordVBO->BufferSubData(0, texCoordsData.size() * sizeof(float), texCoordsData.data());
    texCoordVBO->UnbindVBO();
    ibo->BufferSubData(0, totalIndices.size() * sizeof(uint32_t), totalIndices.data());
    ibo->UnbindVBO();
}

void ElectricBeamSceneProxy::CreateBeamSkins()
{
    if (mBeamSkin) {
        RuntimeGeneratedMeshPool::GetInstance()->TryToFreeMemory(mBeamSkin);
        mBeamSkin.reset();
    }

    int32_t totalCountOfVertices = 0, totalCountOfIndices = 0;
    for (int i = 0; i < mBeamCount; ++i) {
        totalCountOfVertices += (mRadialSegments + 1) * (mLengthSegments + 1);
        totalCountOfIndices += mRadialSegments * mLengthSegments * 6;
    }

    const auto vertexAttributes = GetShader()->GetVertexAttributes();
    mBeamMeshPoolParams.mVertexAttributes = vertexAttributes;
    mBeamMeshPoolParams.mMaxVerticesCount = totalCountOfVertices;
    mBeamMeshPoolParams.mMaxIndicesCount = totalCountOfIndices;
    mBeamSkin = RuntimeGeneratedMeshPool::GetInstance()->GetOrAllocateResource(mBeamMeshPoolParams);
}

void ElectricBeamSceneProxy::DestroyBeamSkins()
{
    if (mBeamSkin) {
        RuntimeGeneratedMeshPool::GetInstance()->TryToFreeMemory(mBeamSkin);
        mBeamSkin.reset();
    }
}

} // namespace Proxy
} // namespace Graphics
