#include "RuntimeGeneratedLineSceneProxy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/SceneViewInfo/SceneView.h"
#include "Core/ResourceManagerCore/Pool/RuntimeGeneratedMeshPool.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace Resources;
using namespace EngineCore;
using namespace Graphics::Renderer;
using namespace Graphics;

namespace Graphics {
namespace Proxy {
RuntimeGeneratedLineSceneProxy::RuntimeGeneratedLineSceneProxy(const RuntimeGeneratedLineComponent* component)
    : StaticMeshSceneProxy(component)
    , mLineBeginWorldSpacePosition(component->GetLineBeginWorldSpacePosition())
    , mLineEndWorldSpacePosition(component->GetLineEndWorldSpacePosition())
    , mLineWidth(component->GetLineWidth())
    , mRtMeshPoolParams(component->GetRuntimeMeshPoolParameters())
{
}

void RuntimeGeneratedLineSceneProxy::PostConstructorInitialize()
{
    static constexpr uint64_t functionId = Hash64_CT("RuntimeGeneratedLineSceneProxy::PostConstructorInitialize");

    ShaderParams shaderParams("RuntimeGeneratedMesh_BaseShader");
    shaderParams.SetMainShaders(
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "runtimeGeneratedMeshVS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "forwardNoLitFS.glsl");

    m_shader = CreateMaterialShader<StaticMeshVertexFactory, SimpleShader>(
        "StaticMeshVertexFactory_SimpleShader_" + mMaterialProxy->MaterialName, shaderParams, mMaterialProxy);

    ShaderParams planarReflectionParams("PlanarReflectionShader");
    planarReflectionParams.SetMainShaders(
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "planarReflectionVS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + "composite_shaders" + SLASH + "forwardNoLitFS.glsl");

    m_planarReflectionShader = CreateMaterialShader<StaticMeshVertexFactory, CapturePlanarReflectionShader>(
        "StaticMeshVertexFactory_CapturePlanarReflectionShader_" + mMaterialProxy->MaterialName,
        planarReflectionParams,
        mMaterialProxy);
    mRtMeshPoolParams.mVertexAttributes = GetShader()->GetVertexAttributes();
    m_skin = RuntimeGeneratedMeshPool::GetInstance()->GetOrAllocateResource(mRtMeshPoolParams);

    if (const auto& deferredShadingSceneRendererSp = GetDeferredShadingSceneRendererWp().lock()) {
        if (const auto& sceneSp = deferredShadingSceneRendererSp->GetInterThreadCommunicationManager().GetSceneWP().lock()) {
            const auto boundingBox = m_skin->GetBoundingBox();
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(
                eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                mSceneProxyId,
                functionId,
                [sceneSp, boundingBox, goID = GetGameObjectId()](
                    std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                    std::weak_ptr<EngineCore::Scene> sceneWp,
                    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                    const auto& engineObject = sceneSp->GetEngineObjectById(goID);
                    ext_assert(engineObject, "Engine object not found by ID");
                    const auto& primitiveComponent = std::static_pointer_cast<PrimitiveComponent>(engineObject);
                    ext_assert(primitiveComponent, "Failed to cast engine object to PrimitiveComponent");
                    primitiveComponent->SetBoundingBox(boundingBox);
                });
        }
    }
}

void RuntimeGeneratedLineSceneProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    UpdateGeometry(viewMatrix);

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
    shader->GetVertexFactoryShader()->SetMatrices(m_worldMatrix, viewMatrix, projectionMatrix);
    m_skin->GetBuffer()->RenderVAO(0, mVerticesCountToRender, GL_TRIANGLE_STRIP);

    if (isCullFaceEnabled) {
        glEnable(GL_CULL_FACE);
    }
}

void RuntimeGeneratedLineSceneProxy::SetLineBeginWorldSpacePosition(const glm::vec3& position)
{
    mLineBeginWorldSpacePosition = position;
    bUpdateLineGeometry = true;
}

void RuntimeGeneratedLineSceneProxy::SetLineEndWorldSpacePosition(const glm::vec3& position)
{
    mLineEndWorldSpacePosition = position;
    bUpdateLineGeometry = true;
}

void RuntimeGeneratedLineSceneProxy::SetLineWidth(const float lineWidth)
{
    mLineWidth = lineWidth;
    bUpdateLineGeometry = true;
}

bool RuntimeGeneratedLineSceneProxy::IsFrustumCullTestNeeded() const
{
    return false;
}

void RuntimeGeneratedLineSceneProxy::UpdateGeometry(const glm::mat4& viewMatrix)
{
    if (bUpdateLineGeometry) {
        auto* const verticesVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
        auto* const textureCoordinatesVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");

        ext_assert(verticesVBO && textureCoordinatesVBO, "VBOs not created for runtime generated line");

        const float halfWidth = mLineWidth * 0.5f;
        const auto& worldForwardVec = glm::normalize(mLineEndWorldSpacePosition - mLineBeginWorldSpacePosition);

        const auto& cameraUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
        const auto& lineBinormalVector = glm::normalize(glm::cross(cameraUpVector, worldForwardVec));

        const auto& viewP1 = mLineBeginWorldSpacePosition - (lineBinormalVector * halfWidth);
        const auto& viewP2 = mLineBeginWorldSpacePosition + (lineBinormalVector * halfWidth);
        const auto& viewP3 = mLineEndWorldSpacePosition - (lineBinormalVector * halfWidth);
        const auto& viewP4 = mLineEndWorldSpacePosition + (lineBinormalVector * halfWidth);

        const auto& texP1 = glm::vec2(0, 1);
        const auto& texP2 = glm::vec2(0, 0);
        const auto& texP3 = glm::vec2(1, 1);
        const auto& texP4 = glm::vec2(1, 0);

        std::vector<float> vertices = std::vector<float>(
            {viewP1.x,
             viewP1.y,
             viewP1.z,
             viewP2.x,
             viewP2.y,
             viewP2.z,
             viewP3.x,
             viewP3.y,
             viewP3.z,
             viewP4.x,
             viewP4.y,
             viewP4.z});

        std::vector<float> texCoords = {texP1.x, texP1.y, texP2.x, texP2.y, texP3.x, texP3.y, texP4.x, texP4.y};

        const size_t verticesBufferSize = sizeof(float) * vertices.size();
        const size_t texCoordsBufferSize = sizeof(float) * texCoords.size();

        verticesVBO->BufferSubData(0, verticesBufferSize, (void*)vertices.data());
        textureCoordinatesVBO->BufferSubData(0, texCoordsBufferSize, (void*)texCoords.data());
        textureCoordinatesVBO->UnbindBuffer();

        bUpdateLineGeometry = false;
    }
}

RenderInfo RuntimeGeneratedLineSceneProxy::GetRenderInfo() const
{
    return RenderInfo{m_shader->GetShaderName()};
}
} // namespace Proxy
} // namespace Graphics
