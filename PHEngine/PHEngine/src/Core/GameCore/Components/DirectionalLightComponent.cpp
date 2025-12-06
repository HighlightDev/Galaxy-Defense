#include "DirectionalLightComponent.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/DirectionalLightSceneProxy.h"
#include "Core/GraphicsCore/Shadow/ProjectedShadowInfo.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtx/quaternion.hpp>

using namespace Graphics;

namespace EngineCore {

DirectionalLightComponent::DirectionalLightComponent(const std::shared_ptr<LightComponentData>& lightComponentData)
    : LightComponent(lightComponentData)
{
    const auto& d_directionalLight = std::static_pointer_cast<DirectionalLightComponentData>(lightComponentData);
    ext_assert(nullptr == mLightRenderData, "DirectionalLightComponent render data is already initialized in constructor");
    mLightRenderData = std::make_shared<DirectionalLightRenderData>(
        d_directionalLight->Direction,
        d_directionalLight->Ambient,
        d_directionalLight->Diffuse,
        d_directionalLight->Specular,
        d_directionalLight->ShadowInfo);
}

DirectionalLightComponent::~DirectionalLightComponent()
{
    PlayerMovedGameThreadEvent::GetInstance()->RemoveListener(PlayerMovedGameThreadEvent::GetInstanceId());
    PhysicsComponentUpdatedGameThreadEvent::GetInstance()->RemoveListener(
        PhysicsComponentUpdatedGameThreadEvent::GetInstanceId());
}

void DirectionalLightComponent::OnRegistered()
{
    LightComponent::OnRegistered();

    if (mLightRenderData->ShadowInfo) {
        PlayerMovedGameThreadEvent::GetInstance()->AddListener(
            std::dynamic_pointer_cast<DirectionalLightComponent>(shared_from_this()));
        PhysicsComponentUpdatedGameThreadEvent::GetInstance()->AddListener(
            std::dynamic_pointer_cast<DirectionalLightComponent>(shared_from_this()));
    }
}

std::shared_ptr<DirectionalLightRenderData> DirectionalLightComponent::GetRenderData() const
{
    return std::static_pointer_cast<DirectionalLightRenderData>(mLightRenderData);
}

std::shared_ptr<LightSceneProxy> DirectionalLightComponent::CreateSceneProxy() const
{
    return std::make_shared<DirectionalLightSceneProxy>(this);
}

void DirectionalLightComponent::UpdateWorldMatrix(const glm::mat4& parentWorldMatrix)
{
    Base::UpdateWorldMatrix(parentWorldMatrix);
}

void DirectionalLightComponent::Tick(float deltaTimeSec)
{
    Base::Tick(deltaTimeSec);

    if (bIsRenderDataDirty && bIsSceneProxyReady.load(std::memory_order::seq_cst)) {
        SyncRenderData();
        bIsRenderDataDirty = false;
    }
}

eComponentType DirectionalLightComponent::GetComponentType() const
{
    return eComponentType::LIGHT_COMPONENT;
}

void DirectionalLightComponent::ProcessEvent(
    const PlayerMovedGameThreadEvent* sender, const PlayerMovedGameThreadEvent::EventData_t& data)
{
    if (const auto playerTranslationOffset = std::get<0>(data).lock()) {
        if (!EngineMath::CheckSimilarityVec3(playerTranslationOffset->Translation, mPlayerTranslationOffset)) {
            mPlayerTranslationOffset = playerTranslationOffset->Translation;
            bIsRenderDataDirty = true;
            if (bIsSceneProxyReady.load(std::memory_order::seq_cst)) {
                SyncRenderData();
                bIsRenderDataDirty = false;
            }
        }
    }
}

void DirectionalLightComponent::ProcessEvent(
    const PhysicsComponentUpdatedGameThreadEvent* sender, const PhysicsComponentUpdatedGameThreadEvent::EventData_t& data)
{
    bIsRenderDataDirty = true;
}

void DirectionalLightComponent::SyncRenderData()
{
    if (const auto& sceneSp = m_sceneWP.lock()) {
        static const uint64_t functionId = Hash("DirectionalLightComponent::SetPlayerPositionOffset");
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            GetObjectId(),
            functionId,
            [lightSceneProxyId = mLightSceneProxyId, playerTranslationOffset = mPlayerTranslationOffset](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRendererSp = sceneRendererWp.lock()) {
                    const auto& lightProxySp = sceneRendererSp->GetLightProxyByProxyId(lightSceneProxyId);
                    ext_assert(lightProxySp, "Light scene proxy not found in DirectionalLightComponent::SyncRenderData");
                    const auto shadowInfo = lightProxySp->GetShadowInfo();
                    if (shadowInfo) {
                        shadowInfo->SetPlayerPositionOffset(playerTranslationOffset);
                        lightProxySp->SetIsTransformationDirty(true);
                        shadowInfo->SetIsShadowMapDirty(true);
                    }
                }
            });
    }
}

} // namespace EngineCore
