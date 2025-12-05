#include "BillboardComponent.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/ComponentData/BillboardComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/BillboardSceneProxy.h"

namespace EngineCore {

BillboardComponent::BillboardComponent(const std::shared_ptr<BillboardComponentData>& data, const BillboardRenderData& renderData)
    : PrimitiveComponent(data->EngineObjectName, data->m_translation, glm::vec3(), data->m_scale)
    , mBillboardExtent(data->m_billboardExtent)
    , m_renderData(renderData)
    , mViewMatrixTransformer(data->mViewMatrixTransformer)
    , mProjectionMatrixTransformer(data->mProjectionMatrixTransformer)
    , mApplyScreenAspectRatio(data->mApplyScreenAspectRatio)
    , mRotationRadians(data->m_rotationRadians)
{
}

BillboardComponent::~BillboardComponent()
{
}

eComponentType BillboardComponent::GetComponentType() const
{
    return PRIMITIVE_COMPONENT;
}

void BillboardComponent::UnpausableTick(float deltaTimeSec)
{
    PrimitiveComponent::UnpausableTick(deltaTimeSec);

    if (bIsSceneProxyReady.load(std::memory_order::seq_cst)
        && (bIsExtentDataDirty || bIsApplyScreenAspectRatioDirty || bIsRotationDirty)) {
        SyncRenderData();
        bIsExtentDataDirty = false;
        bIsApplyScreenAspectRatioDirty = false;
        bIsRotationDirty = false;
    }
}

std::shared_ptr<PrimitiveSceneProxy> BillboardComponent::CreateSceneProxy() const
{
    return std::make_shared<BillboardSceneProxy>(this);
}

void BillboardComponent::SetBillboardExtent(const float extent)
{
    if (mBillboardExtent != extent) {
        mBillboardExtent = extent;
        bIsExtentDataDirty = true;
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
             rotationRadians = mRotationRadians](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& sceneRendererSp = sceneRendererWp.lock()) {
                    if (const auto& billboardProxySp = std::static_pointer_cast<BillboardSceneProxy>(
                            sceneRendererSp->GetPrimitiveProxyByProxyId(sceneProxyId))) {
                        billboardProxySp->SetBillboardExtent(billboardExtent);
                        billboardProxySp->SetApplyScreenAspectRatio(applyScreenAspectRatio);
                        billboardProxySp->SetRotationRadians(rotationRadians);
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
        bIsApplyScreenAspectRatioDirty = true;
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
        bIsRotationDirty = true;
    }
}

float BillboardComponent::GetRotationRadians() const
{
    return mRotationRadians;
}
} // namespace EngineCore
