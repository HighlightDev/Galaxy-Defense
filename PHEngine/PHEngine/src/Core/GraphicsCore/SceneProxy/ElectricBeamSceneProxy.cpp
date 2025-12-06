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
    , mBeamColor(component->GetBeamColor())
    , mBeamCount(component->GetBeamCount())
    , mRenderMode(component->GetRenderMode())
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
        // Update geometry for all beams
        for (int i = 0; i < mBeamCount; ++i) {
            UpdateBeamGeometry(i);
        }
        bUpdateBeamGeometry = false;
    }

    const auto& shader = GetShader();

    GLboolean isCullFaceEnabled;
    glGetBooleanv(GL_CULL_FACE, &isCullFaceEnabled);

    if (isCullFaceEnabled) {
        glDisable(GL_CULL_FACE);
    }

    const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(shader->GetShaderName());
    if (needToRebindShader) {
        shader->ExecuteShader();
    }

    shader->GetMaterialShader()->LoadUniformValues(mMaterialProxy, activeBindedState);

    // Render each beam
    for (size_t i = 0; i < mBeamSkins.size(); ++i) {
        if (mBeamSkins[i]) {
            auto modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, i * 2.0f, 0.0f));
            shader->GetVertexFactoryShader()->SetMatrices(modelMatrix, viewMatrix, projectionMatrix);
            const auto& buffer = mBeamSkins[i]->GetBuffer();
            buffer->RenderVAO(GL_TRIANGLES);
        }
    }

    if (isCullFaceEnabled) {
        glEnable(GL_CULL_FACE);
    }
}

bool ElectricBeamSceneProxy::IsDeferred() const
{
    return false;
}

void ElectricBeamSceneProxy::SetBeamColor(const glm::vec3& color)
{
    mBeamColor = color;
    bUpdateBeamGeometry = true;
}

void ElectricBeamSceneProxy::SetBeamCount(int count)
{
    if (count < 1)
        count = 1;
    if (count > 20)
        count = 20;

    mBeamCount = count;
    DestroyBeamSkins();
    CreateBeamSkins();
    bUpdateBeamGeometry = true;
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

void ElectricBeamSceneProxy::UpdateBeamGeometry(int beamIndex)
{
    if (beamIndex < 0 || beamIndex >= static_cast<int>(mBeamSkins.size()) || mMeshData.empty()
        || beamIndex >= static_cast<int>(mMeshData.size())) {
        return;
    }

    const auto& vertices = std::get<0>(mMeshData[beamIndex]);
    const auto& indices = std::get<1>(mMeshData[beamIndex]);

    if (!vertices.empty() && !indices.empty() && mBeamSkins[beamIndex]) {
        // Update mesh data
        auto& buffer = mBeamSkins[beamIndex]->GetBuffer();

        // Convert BeamVertex to the format expected by the mesh buffer
        std::vector<float> vertexData;
        std::vector<float> texCoordsData;
        std::vector<float> normalData;
        vertexData.reserve(vertices.size() * 3);
        texCoordsData.reserve(vertices.size() * 2);
        normalData.reserve(vertices.size() * 3);

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

        const auto& positionVBO = buffer->GetVboByAttribArrayIndexName("VertexPosition");
        const auto& normalVBO = buffer->GetVboByAttribArrayIndexName("VertexNormal");
        const auto& texCoordVBO = buffer->GetVboByAttribArrayIndexName("VertexTexCoords");
        const auto& ibo = buffer->GetIBO();

        positionVBO->BufferSubData(0, vertexData.size() * sizeof(float), vertexData.data());
        normalVBO->BufferSubData(0, normalData.size() * sizeof(float), normalData.data());
        texCoordVBO->BufferSubData(0, texCoordsData.size() * sizeof(float), texCoordsData.data());
        texCoordVBO->UnbindVBO();
        ibo->BufferSubData(0, indices.size() * sizeof(uint32_t), indices.data());
        ibo->UnbindVBO();
    }
}

void ElectricBeamSceneProxy::CreateBeamSkins()
{
    mBeamMeshPoolParams.clear();
    mBeamSkins.clear();

    mBeamMeshPoolParams.resize(mBeamCount);
    mBeamSkins.resize(mBeamCount);

    const auto vertexAttributes = GetShader()->GetVertexAttributes();

    for (int i = 0; i < mBeamCount; ++i) {
        mBeamMeshPoolParams[i].mVertexAttributes = vertexAttributes;
        mBeamMeshPoolParams[i].mMaxVerticesCount = (mRadialSegments + 1) * (mLengthSegments + 1);
        mBeamMeshPoolParams[i].mMaxIndicesCount = mRadialSegments * mLengthSegments * 6;

        mBeamSkins[i] = RuntimeGeneratedMeshPool::GetInstance()->GetOrAllocateResource(mBeamMeshPoolParams[i]);
    }
}

void ElectricBeamSceneProxy::DestroyBeamSkins()
{
    mBeamSkins.clear();
    mBeamMeshPoolParams.clear();
}

} // namespace Proxy
} // namespace Graphics
