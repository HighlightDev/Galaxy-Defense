#include "BillboardComponent.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/BillboardComponentLuaProxy.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/BillboardSceneProxy.h"

using namespace EngineCore::Scripts;

namespace EngineCore {

BillboardComponent::BillboardComponent(const std::shared_ptr<BillboardComponentData>& data, const BillboardRenderData& renderData)
    : PrimitiveComponent(
        data->EngineObjectName, data->m_translation, glm::vec3(), data->m_scale, data->mIsEnabled, data->mIsVisible)
    , mBillboardExtent(data->m_billboardExtent)
    , m_renderData(renderData)
    , mViewMatrixTransformer(data->mViewMatrixTransformer)
    , mProjectionMatrixTransformer(data->mProjectionMatrixTransformer)
    , mApplyScreenAspectRatio(data->mApplyScreenAspectRatio)
    , mRotationRadians(data->m_rotationRadians)
    , mIsFlipped(data->mIsFlipped)
{
}

BillboardComponent::~BillboardComponent()
{
}

eComponentType BillboardComponent::GetComponentType() const
{
    return PRIMITIVE_COMPONENT;
}

void BillboardComponent::UnpausableTick(float deltaTimeSec, const float playSpeed)
{
    PrimitiveComponent::UnpausableTick(deltaTimeSec, playSpeed);

    if (bIsSceneProxyReady.load(std::memory_order::seq_cst) && bIsRenderDataDirty) {
        SyncRenderData();
        bIsRenderDataDirty = false;
    }
}

std::shared_ptr<PrimitiveSceneProxy> BillboardComponent::CreateSceneProxy() const
{
    return std::make_shared<BillboardSceneProxy>(this);
}

std::shared_ptr<Scripts::LuaProxy> BillboardComponent::ReplicateLuaProxy()
{
    return std::make_shared<BillboardComponentLuaProxy>(std::static_pointer_cast<BillboardComponent>(shared_from_this()));
}

void BillboardComponent::SetBillboardExtent(const float extent)
{
    if (mBillboardExtent != extent) {
        mBillboardExtent = extent;
        bIsRenderDataDirty = true;
    }
}

float BillboardComponent::GetBillboardExtent() const
{
    return mBillboardExtent;
}

void BillboardComponent::SetIsEnabled(const bool bEnabled)
{
    PrimitiveComponent::SetIsEnabled(bEnabled);

    const auto& material = GetMaterial();
    if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType()) {
        material->SetIsEnabled(bEnabled);
    }
}

void BillboardComponent::SetIsVisible(bool isVisible)
{
    PrimitiveComponent::SetIsVisible(isVisible);

    const auto& material = GetMaterial();
    if (IMaterial::eMaterialType::DYNAMIC == material->GetMaterialType()) {
        material->SetIsEnabled(isVisible);
    }
}

std::shared_ptr<IMaterial> BillboardComponent::GetMaterial() const
{
    std::shared_ptr<IMaterial> materialResult = nullptr;
    if (const auto& sceneSP = m_sceneWP.lock()) {
        materialResult = sceneSP->GetMaterialByProxyId(m_renderData.mMaterialProxy->GetSceneProxyId());
    }
    ext_assert(materialResult != nullptr, "BillboardComponent::GetMaterial: materialResult is null");
    return materialResult;
}

void BillboardComponent::SyncRenderData()
{
    if (const auto& sceneSp = m_sceneWP.lock()) {
        static const uint64_t functionId = Hash("BillboardComponent:SetBillboardExtent");
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            GetObjectId(),
            functionId,
            [sceneProxyId = mSceneProxyId,
             billboardExtent = mBillboardExtent,
             applyScreenAspectRatio = mApplyScreenAspectRatio,
             rotationRadians = mRotationRadians,
             isFlipped = mIsFlipped](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRendererSp = sceneRendererWp.lock()) {
                    if (const auto& billboardProxySp = std::static_pointer_cast<BillboardSceneProxy>(
                            sceneRendererSp->GetPrimitiveProxyByProxyId(sceneProxyId))) {
                        billboardProxySp->SetBillboardExtent(billboardExtent);
                        billboardProxySp->SetApplyScreenAspectRatio(applyScreenAspectRatio);
                        billboardProxySp->SetRotationRadians(rotationRadians);
                        billboardProxySp->SetIsFlipped(isFlipped);
                    }
                }
            });
    }
}

std::function<glm::mat4(const glm::mat4&)> BillboardComponent::GetViewMatrixTransformer() const
{
    return mViewMatrixTransformer;
}

std::function<glm::mat4(const glm::mat4&)> BillboardComponent::GetProjectionMatrixTransformer() const
{
    return mProjectionMatrixTransformer;
}

void BillboardComponent::SetApplyScreenAspectRatio(const bool apply)
{
    if (mApplyScreenAspectRatio != apply) {
        mApplyScreenAspectRatio = apply;
        bIsRenderDataDirty = true;
    }
}

bool BillboardComponent::GetApplyScreenAspectRatio() const
{
    return mApplyScreenAspectRatio;
}

void BillboardComponent::SetRotationRadians(const float rotationRadians)
{
    if (!EngineMath::FloatsNearEqual(rotationRadians, mRotationRadians)) {
        mRotationRadians = rotationRadians;
        bIsRenderDataDirty = true;
    }
}

float BillboardComponent::GetRotationRadians() const
{
    return mRotationRadians;
}

void BillboardComponent::SetIsFlipped(const bool isFlipped)
{
    if (mIsFlipped != isFlipped) {
        mIsFlipped = isFlipped;
        bIsRenderDataDirty = true;
    }
}

bool BillboardComponent::GetIsFlipped() const
{
    return mIsFlipped;
}
} // namespace EngineCore
