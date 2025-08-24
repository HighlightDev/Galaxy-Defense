#include "PlanarReflectionComponent.h"

#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/ComponentData/PlanarReflectionComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/PlanarReflectionProxy.h"
#include "Core/GraphicsCore/Texture/Texture2d.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace Graphics;
using namespace EngineMath;

namespace EngineCore {

PlanarReflectionComponent::PlanarReflectionComponent(const std::shared_ptr<PlanarReflectionComponentData>& data)
    : SceneComponent(data->EngineObjectName, data->m_translation, data->m_eulerRotationDegrees, data->m_scale)
    , mReflectionPlane()
    , mOwnerCameraWp(data->m_ownerCamera)
    , mRenderTargetViewPortInfo(data->m_fboViewPortInfo)
    , mPlanarReflectionDeferredController(
          std::make_shared<DeferredResourceController<std::shared_ptr<ITexture>, eDeferredResourceType::TEXTURE>>())
{
}

PlanarReflectionComponent::~PlanarReflectionComponent()
{
}

void PlanarReflectionComponent::SetIsSceneProxyReady(const bool isReady)
{
    bIsSceneProxyReady.store(isReady, std::memory_order::seq_cst);
}

bool PlanarReflectionComponent::IsSceneProxyReady() const
{
    return bIsSceneProxyReady.load(std::memory_order::seq_cst);
}

void PlanarReflectionComponent::UpdateReflectionPlane()
{
    glm::vec4 positionOnPlane = m_relativeMatrix * glm::vec4(0, 0, 0, 1);
    const glm::vec3 defaultNormal = -AXIS_UP;
    glm::vec4 normal = m_relativeMatrix * glm::vec4(defaultNormal, 0.0f);

    float d = glm::dot(normal, positionOnPlane);
    mReflectionPlane = glm::vec4(glm::vec3(normal), d);
}

std::shared_ptr<DeferredResourceController<std::shared_ptr<ITexture>, eDeferredResourceType::TEXTURE>>
PlanarReflectionComponent::GetPlanarReflectionDeferredController() const
{
    return mPlanarReflectionDeferredController;
}

::Graphics::ViewPortInfo PlanarReflectionComponent::GetRenderTargetViewPortInfo() const
{
    return mRenderTargetViewPortInfo;
}

std::shared_ptr<PlanarReflectionProxy> PlanarReflectionComponent::CreatePlanarReflectionProxy()
{
    return std::make_shared<PlanarReflectionProxy>(this);
}

void PlanarReflectionComponent::OnPostInitialized()
{
    if (const auto& sceneSp = m_sceneWP.lock()) {
        sceneSp->RegisterDeferredResourceCreator(
            std::dynamic_pointer_cast<PlanarReflectionComponent>(shared_from_this()), GetEngineObjectName());
    }
}

void PlanarReflectionComponent::PostLevelInit()
{
    SceneComponent::PostLevelInit();

    static const uint64_t functionId = Hash("PlanarReflectionComponent::PostLevelInit");
    if (const auto& sceneSp = m_sceneWP.lock()) {
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            GetObjectId(),
            functionId,
            [weak = weak_from_this(), planarReflectionSceneProxyId = mPlanarReflectionSceneProxyId](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRendererSp = sceneRendererWp.lock()) {
                    if (const auto& componentPtr = weak.lock()) {
                        const auto planarReflectionComponentPtr
                            = std::static_pointer_cast<PlanarReflectionComponent>(componentPtr);
                        const auto& reflectionSp
                            = sceneRendererSp->GetPlanarReflectionProxyByProxyId(planarReflectionSceneProxyId);
                        assert(reflectionSp);
                        const auto& proxySp = std::static_pointer_cast<PlanarReflectionProxy>(reflectionSp);
                        auto resourceTexture = proxySp->GetPlanarReflectionTexture();
                        planarReflectionComponentPtr->GetPlanarReflectionDeferredController()
                            ->GetDeferredResource(); // Just in case deferred resource wasn't initialized
                        planarReflectionComponentPtr->GetPlanarReflectionDeferredController()->SetResource(resourceTexture);
                    }
                }
            });
    }
}

void PlanarReflectionComponent::Tick(const float deltaTime)
{
    if (mIsEnabled) {
        if (bTransformationDirty && bIsSceneProxyReady.load(std::memory_order::seq_cst)) {
            UpdateRelativeMatrix();
            UpdateReflectionPlane();
            bIsRenderDataDirty = true;
        }

        if (bIsRenderDataDirty && bIsSceneProxyReady.load(std::memory_order::seq_cst)) {
            SyncDataWithRenderThread();
            bIsRenderDataDirty = false;
        }
    }
}

std::weak_ptr<ACamera> PlanarReflectionComponent::GetOwnerCameraWp() const
{
    return mOwnerCameraWp;
}

int32_t PlanarReflectionComponent::GetSceneProxyId() const
{
    return mPlanarReflectionSceneProxyId;
}

glm::vec4 PlanarReflectionComponent::GetReflectionPlane() const
{
    return mReflectionPlane;
}

void PlanarReflectionComponent::SetSceneProxyId(const int32_t sceneProxyId)
{
    mPlanarReflectionSceneProxyId = sceneProxyId;
}

eComponentType PlanarReflectionComponent::GetComponentType() const
{
    return PLANAR_REFLECTION_COMPONENT;
}

std::shared_ptr<IDeferredResource> PlanarReflectionComponent::GetDeferredResource()
{
    return mPlanarReflectionDeferredController->GetDeferredResource();
}

void PlanarReflectionComponent::SyncDataWithRenderThread()
{
    static const uint64_t functionId = Hash("PlanarReflectionComponent::SyncDataWithRenderThread");
    if (const auto& sceneSp = m_sceneWP.lock()) {
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            GetObjectId(),
            functionId,
            [weak = weak_from_this(),
             planarReflectionSceneProxyId = mPlanarReflectionSceneProxyId,
             reflectionPlane = mReflectionPlane](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& componentPtr = weak.lock()) {
                    if (const auto& sceneRendererSp = sceneRendererWp.lock()) {
                        const auto& reflectionSp = std::static_pointer_cast<PlanarReflectionProxy>(
                            sceneRendererSp->GetPlanarReflectionProxyByProxyId(planarReflectionSceneProxyId));
                        if (reflectionSp) {
                            reflectionSp->SetReflectionPlane(reflectionPlane);
                        }
                    }
                }
            });
    }
}
} // namespace EngineCore