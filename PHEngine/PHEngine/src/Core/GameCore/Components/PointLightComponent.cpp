#include "PointLightComponent.h"

#include "Core/GameCore/Components/ComponentData/PointLightComponentData.h"
#include "Core/GameCore/Event/PhysicsComponentUpdatedEvent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/PointLightSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace Graphics;

namespace EngineCore {

PointLightComponent::PointLightComponent(const std::shared_ptr<LightComponentData>& lightComponentData)
    : LightComponent(lightComponentData)
{
    const auto& d_pointLight = std::static_pointer_cast<PointLightComponentData>(lightComponentData);
    assert(nullptr == mLightRenderData);
    mLightRenderData = std::make_shared<PointLightRenderData>(
        d_pointLight->Attenuation,
        d_pointLight->RadianceRadius,
        d_pointLight->Ambient,
        d_pointLight->Diffuse,
        d_pointLight->Specular,
        d_pointLight->ShadowInfo);
}

PointLightComponent::~PointLightComponent()
{
    if (mLightRenderData->ShadowInfo) {
        PhysicsComponentUpdatedGameThreadEvent::GetInstance()->RemoveListener(
            PhysicsComponentUpdatedGameThreadEvent::GetInstanceId());
        KinematicBodyMovedGameThreadEvent::GetInstance()->RemoveListener(KinematicBodyMovedGameThreadEvent::GetInstanceId());
        PlayerMovedGameThreadEvent::GetInstance()->RemoveListener(PlayerMovedGameThreadEvent::GetInstanceId());
    }
}

void PointLightComponent::Initialize()
{
    if (mLightRenderData->ShadowInfo) {
        const auto thisSp = std::dynamic_pointer_cast<PointLightComponent>(shared_from_this());
        PhysicsComponentUpdatedGameThreadEvent::GetInstance()->AddListener(thisSp);
        KinematicBodyMovedGameThreadEvent::GetInstance()->AddListener(thisSp);
        PlayerMovedGameThreadEvent::GetInstance()->AddListener(thisSp);
    }
}

std::shared_ptr<PointLightRenderData> PointLightComponent::GetRenderData() const
{
    return std::static_pointer_cast<PointLightRenderData>(mLightRenderData);
}

std::shared_ptr<LightSceneProxy> PointLightComponent::CreateSceneProxy() const
{
    return std::make_shared<PointLightSceneProxy>(this);
}

eComponentType PointLightComponent::GetComponentType() const
{
    return LIGHT_COMPONENT;
}

void PointLightComponent::Tick(const float deltaTime)
{
    Base::Tick(deltaTime);
}

void PointLightComponent::ProcessEvent(
    const PhysicsComponentUpdatedGameThreadEvent* sender, const PhysicsComponentUpdatedGameThreadEvent::EventData_t& data)
{
    static const uint64_t functionId = Hash("PointLightComponent: Set shadowInfo->bMustUpdateShadowmap");

    NotifySceneProxyThatShadowmapIsDirty(functionId);
}

void PointLightComponent::ProcessEvent(
    const KinematicBodyMovedGameThreadEvent* sender, const KinematicBodyMovedGameThreadEvent::EventData_t& data)
{
    static const uint64_t functionId = Hash("PointLightComponent: Set shadowInfo->bMustUpdateShadowmap");

    NotifySceneProxyThatShadowmapIsDirty(functionId);
}

void PointLightComponent::ProcessEvent(
    const PlayerMovedGameThreadEvent* sender, const PlayerMovedGameThreadEvent::EventData_t& data)
{
    static const uint64_t functionId = Hash("PointLightComponent: Set shadowInfo->bMustUpdateShadowmap");

    NotifySceneProxyThatShadowmapIsDirty(functionId);
}

void PointLightComponent::NotifySceneProxyThatShadowmapIsDirty(const uint64_t& functionId)
{
    if (const auto& sceneSp = m_sceneWP.lock()) {
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            GetObjectId(),
            functionId,
            [weak = weak_from_this()](std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
    std::weak_ptr<EngineCore::Scene> sceneWp,
    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRendererSp = sceneRendererWp.lock()) {
                    if (const auto& componentPtr = weak.lock()) {
                        const auto pointLightComponentPtr = std::static_pointer_cast<PointLightComponent>(componentPtr);
                        const auto& lightProxySp
                            = sceneRendererSp->GetLightProxyByProxyId(pointLightComponentPtr->GetLightSceneProxyId());
                        const auto& shadowInfo = lightProxySp->GetShadowInfo();
                        if (shadowInfo) {
                            shadowInfo->SetIsShadowMapDirty(true);
                        }
                    }
                }
            });
    }
}
} // namespace EngineCore