#include "CubemapSceneProxy.h"

#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlasFactory.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/SimplePrimitivePoolParameters.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/ResourceManagerCore/Pool/SimplePrimitivePool.h"
#include "Core/ResourceManagerCore/SimpleMeshType.h"

using namespace Graphics::Renderer;
using namespace Graphics::OpenGL;
using namespace EngineCore;
using namespace Resources;

namespace Graphics {
namespace Proxy {
CubemapSceneProxy::CubemapSceneProxy(const CubemapComponent* component)
    : PrimitiveSceneProxy(component, nullptr)
    , m_shaderCubemap(nullptr)
    , m_textureObtainer(component->GetRenderData().m_texture)
{
}

CubemapSceneProxy::~CubemapSceneProxy()
{
}

void CubemapSceneProxy::CleanUp()
{
    PrimitiveSceneProxy::CleanUp();

    if (m_shaderCubemap) {
        ShaderPool::GetInstance()->TryToFreeMemory(m_shaderCubemap);
        m_shaderCubemap = nullptr;
    }
}

void CubemapSceneProxy::PostConstructorInitialize()
{
    PrimitiveSceneProxy::PostConstructorInitialize();
    static constexpr uint64_t functionId = Hash64_CT("CubemapSceneProxy::PostConstructorInitialize");

    const ShaderParams shaderParams(
        "Cubemap Shader",
        FolderManager::GetInstance()->GetShadersPath() + "cubemapRendererVS.glsl",
        FolderManager::GetInstance()->GetShadersPath() + "cubemapRendererFS.glsl");
    m_shaderCubemap = ShaderPool::GetInstance()->template GetOrAllocateResource<CubemapShader>(shaderParams);

    SimplePrimitivePoolParameters poolParams;
    poolParams.mSimplePrimitiveType = SimplePrimitiveType::CUBE;
    poolParams.mVertexAttributes.emplace_back(std::make_shared<StandartAttributeData<eAttribArrayIndex::VertexPosition>>(0));
    m_skin = SimplePrimitivePool::GetInstance()->GetOrAllocateResource(poolParams);

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
                    assert(engineObject);
                    const auto& primitiveComponent = std::static_pointer_cast<PrimitiveComponent>(engineObject);
                    assert(primitiveComponent);
                    primitiveComponent->SetBoundingBox(boundingBox);
                });
        }
    }
}

std::shared_ptr<IShader> CubemapSceneProxy::GetShader() const
{
    return m_shaderCubemap;
}

void CubemapSceneProxy::Render(
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix,
    ActiveBindedState& activeBindedState)
{
    const std::shared_ptr<TextureAtlasHandler>& texHandler
        = TextureAtlasFactory::GetInstance()->GetTextureAtlasCellByRequestId(m_textureObtainer.MyRequestId);
    if (texHandler && texHandler->GetTextureType() == eTextureType::TEXTURE_CUBE) {
        const auto& texture = texHandler->GetAtlasResource();
        auto cubemapShader = std::static_pointer_cast<CubemapShader>(m_shaderCubemap);

        const bool needToRebindShader = activeBindedState.TryUpdateActiveShaderName(cubemapShader->GetShaderName());
        if (needToRebindShader) {
            cubemapShader->ExecuteShader();
        }
        int32_t slot = activeBindedState.GetBindedSlotIndexByTextureId(texture->GetTextureDescriptor());
        if (slot == -1) {
            slot = activeBindedState.OccupyTextureSlot(texture->GetTextureDescriptor());
            texture->BindTexture(slot);
        }
        cubemapShader->SetTexture(slot);
        cubemapShader->SetTransformMatrices(m_relativeMatrix, viewMatrix, projectionMatrix);
        m_skin->GetBuffer()->RenderVAO(GL_TRIANGLES);
    }
}

bool CubemapSceneProxy::IsDeferred() const
{
    return false;
}

eMeshFacing CubemapSceneProxy::GetMeshFrontFace() const
{
    return eMeshFacing::COUNTER_CLOCK_WISE;
}

RenderInfo CubemapSceneProxy::GetRenderInfo() const
{
    return RenderInfo{m_shaderCubemap->GetShaderName()};
}

} // namespace Proxy
} // namespace Graphics
