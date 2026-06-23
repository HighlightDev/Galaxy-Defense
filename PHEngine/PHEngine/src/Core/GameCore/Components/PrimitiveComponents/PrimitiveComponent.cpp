#include "PrimitiveComponent.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/BoundingBoxBuilder.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"

using namespace Graphics::Renderer;
using namespace Graphics::Proxy;

namespace EngineCore {

PrimitiveComponent::PrimitiveComponent(
    const std::string& gameObjectName,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale,
    const bool isEnabled,
    const bool isVisible)
    : SceneComponent(gameObjectName, translation, rotation, scale, isEnabled)
    , mBoundingBox(BoundingBox3D())
    , mIsVisible(std::make_shared<EngineObjectProperty<bool>>(isVisible, "p_isVisible"))
{
    /******  HOOKS ****/
    AddEngineProperty(mIsVisible);
    /******  HOOKS ****/
}

PrimitiveComponent::~PrimitiveComponent()
{
}

void PrimitiveComponent::SetSceneProxyId(const int32_t proxyId)
{
    mSceneProxyId = proxyId;
}

int32_t PrimitiveComponent::GetSceneProxyId() const
{
    return mSceneProxyId;
}

eComponentType PrimitiveComponent::GetComponentType() const
{
    return PRIMITIVE_COMPONENT;
}

void PrimitiveComponent::UnpausableTick(const float deltaTimeSec, const float playSpeed)
{
    SceneComponent::UnpausableTick(deltaTimeSec, playSpeed);

    // Transform changes are no longer synced here — they are shipped from UpdateWorldMatrix (transform path).
    // SyncRenderData only pushes the state flags below, so bTransformationDirty must not gate it.
    if (bIsEnabledStateDirty || bIsVisibleStateDirty || bIsSortOrderStateDirty || bIsBloomStateDirty || bIsDepthTestStateDirty
        || bIsOutlineStateDirty) {
        SyncRenderData();
    }
}

void PrimitiveComponent::SetIsEnabled(const bool bEnabled)
{
    if (mIsEnabled->GetValue() != bEnabled) {
        mIsEnabled->SetValue(bEnabled, false);
        bIsEnabledStateDirty = true;
    }
}

void PrimitiveComponent::SetIsVisible(bool isVisible)
{
    if (isVisible != mIsVisible->GetValue()) {
        mIsVisible->SetValue(isVisible);
        bIsVisibleStateDirty = true;
    }
}

bool PrimitiveComponent::IsVisible() const
{
    return mIsVisible->GetValue();
}

BoundingBox3D PrimitiveComponent::GetBoundingBox() const
{
    return mBoundingBox;
}

void PrimitiveComponent::SetSortOrderValue(const int32_t orderValue)
{
    if (mSortOrderValue != orderValue) {
        mSortOrderValue = orderValue;
        bIsSortOrderStateDirty = true;
    }
}

int32_t PrimitiveComponent::GetSortOrderValue() const
{
    return mSortOrderValue;
}

void PrimitiveComponent::SetIsSceneProxyReady(const bool isReady)
{
    bIsSceneProxyReady.store(isReady, std::memory_order::seq_cst);
}

bool PrimitiveComponent::IsSceneProxyReady() const
{
    return bIsSceneProxyReady.load(std::memory_order::seq_cst);
}

BoundingBox3D PrimitiveComponent::GetTransformedBoundingBox() const
{
    return BoundingBoxBuilder::GetTransformedBoundingBox(mBoundingBox, m_worldMatrix);
}

void PrimitiveComponent::SetBoundingBox(const BoundingBox3D& boundingBox)
{
    mBoundingBox = boundingBox;
    SetIsTransformationDirty(true); // Update transform for bounding box and sync with render thread
}

void PrimitiveComponent::SetCanBloomBeApplied(const bool value)
{
    if (mCanBloomBeApplied != value) {
        mCanBloomBeApplied = value;
        bIsBloomStateDirty = true;
    }
}

bool PrimitiveComponent::CanBloomBeApplied() const
{
    return mCanBloomBeApplied;
}

void PrimitiveComponent::SetIsOutlineApplied(const bool value)
{
    if (mIsOutlineApplied != value) {
        mIsOutlineApplied = value;
        if (value) {
            // The outline matrix is only computed inside UpdateWorldMatrix when mIsOutlineApplied is true.
            // If the object is stationary the root component is not dirty, so UpdateWorldMatrix won't run
            // before SyncRenderData sends the (stale) m_outlineMatrix to the render thread.
            // Force a correct computation right now using the same parentWorldMatrix that UpdateTransform uses.
            if (const auto ownerSp = GetOwner().lock()) {
                ownerSp->UpdateTransform(true);
            }
        }
        SetIsTransformationDirty(true);
        bIsOutlineStateDirty = true;
    }
}

bool PrimitiveComponent::GetIsOutlineApplied() const
{
    return mIsOutlineApplied;
}

bool PrimitiveComponent::IsDepthWriteMaskEnabled() const
{
    return mDepthWriteMaskEnabled;
}

void PrimitiveComponent::SetDepthWriteMaskEnabled(const bool isEnabled)
{
    if (mDepthWriteMaskEnabled != isEnabled) {
        mDepthWriteMaskEnabled = isEnabled;
        bIsDepthTestStateDirty = true;
    }
}

void PrimitiveComponent::UpdateWorldMatrix(const glm::mat4& parentWorldMatrix)
{
    SceneComponent::UpdateWorldMatrix(parentWorldMatrix);

    if (bIsSceneProxyReady.load(std::memory_order::seq_cst)) {
        if (const auto& sceneSP = m_sceneWP.lock()) {

            // Reaching UpdateWorldMatrix already means the world matrix was just recomputed: either this component is
            // transform-dirty, or its owning actor force-updated the whole hierarchy because a parent moved. In both
            // cases the new matrix must be pushed to the render thread. Instead of posting a per-object job (plus a
            // separate frustum-reset job) every frame, accumulate into the scene's per-frame batch; the whole batch
            // is flushed as a single render-thread job from Scene::UnpausableTick.
            if (IsWorldMatrixComputed()) {
                sceneSP->EnqueuePrimitiveTransformUpdate(
                    mSceneProxyId, m_worldMatrix, m_outlineMatrix, GetTransformedBoundingBox(), mBoundingBox.GetOrigin());
            }
        }
    }

    SetIsTransformationDirty(!bIsSceneProxyReady.load(std::memory_order::seq_cst));
}

void PrimitiveComponent::SyncRenderData()
{
    if (bIsSceneProxyReady.load(std::memory_order::seq_cst)) {
        if (const auto& sceneSP = m_sceneWP.lock()) {
            if (const auto& sceneRendererSp = sceneSP->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {

                if (IsWorldMatrixComputed() && (bIsEnabledStateDirty || bIsVisibleStateDirty)) {
                    sceneRendererSp->ResetPrimitiveFrustumTestResult(mSceneProxyId);
                }

                if (bIsEnabledStateDirty) {
                    static const uint64_t functionId = Hash("PrimitiveComponent:UpdatePrimitiveComponentEnable_GameThread");
                    sceneRendererSp->UpdatePrimitiveComponentEnable_OnRenderThread(
                        mSceneProxyId, GetObjectId(), functionId, mIsEnabled->GetValue());
                    bIsEnabledStateDirty = false;
                }

                if (bIsVisibleStateDirty) {
                    static const uint64_t functionId
                        = Hash("PrimitiveComponent::UpdatePrimitiveComponentVisibility_OnRenderThread()");
                    sceneRendererSp->UpdatePrimitiveComponentVisibility_OnRenderThread(
                        mSceneProxyId, GetObjectId(), functionId, mIsVisible->GetValue());
                    bIsVisibleStateDirty = false;
                }

                if (bIsSortOrderStateDirty) {
                    static constexpr uint64_t functionId
                        = Hash64_CT("PrimitiveComponent::UpdatePrimitiveComponentSortOrderValue_OnRenderThread()");
                    sceneRendererSp->UpdatePrimitiveComponentSortOrderValue_OnRenderThread(
                        mSceneProxyId, GetObjectId(), functionId, mSortOrderValue);
                    bIsSortOrderStateDirty = false;
                }

                if (bIsBloomStateDirty) {
                    static constexpr uint64_t functionId = Hash64_CT("PrimitiveComponent::UpdateBloomState_OnRenderThread()");
                    sceneSP->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                        eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                        GetObjectId(),
                        functionId,
                        [sceneProxyId = mSceneProxyId, sceneRendererSp, canBloomBeApplied = mCanBloomBeApplied](
                            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                            std::weak_ptr<EngineCore::Scene> sceneWp,
                            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                            const auto& primitiveSp = sceneRendererSp->GetPrimitiveProxyByProxyId(sceneProxyId);
                            if (primitiveSp) {
                                primitiveSp->SetCanBloomBeApplied(canBloomBeApplied);
                            }
                        });
                    bIsBloomStateDirty = false;
                }

                if (bIsDepthTestStateDirty) {
                    static constexpr uint64_t functionId = Hash64_CT("PrimitiveComponent::UpdateDepthTestState_OnRenderThread()");
                    sceneSP->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                        eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                        GetObjectId(),
                        functionId,
                        [sceneProxyId = mSceneProxyId, sceneRendererSp, isDepthTestEnabled = mDepthWriteMaskEnabled](
                            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                            std::weak_ptr<EngineCore::Scene> sceneWp,
                            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                            const auto& primitiveSp = sceneRendererSp->GetPrimitiveProxyByProxyId(sceneProxyId);
                            if (primitiveSp) {
                                primitiveSp->SetDepthWriteMaskEnabled(isDepthTestEnabled);
                            }
                        });
                    bIsDepthTestStateDirty = false;
                }

                if (bIsOutlineStateDirty) {
                    static constexpr uint64_t functionId = Hash64_CT("PrimitiveComponent::UpdateOutlineState_OnRenderThread()");
                    sceneSP->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                        eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                        GetObjectId(),
                        functionId,
                        [sceneProxyId = mSceneProxyId, isOutlineApplied = mIsOutlineApplied, sceneRendererSp](
                            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                            std::weak_ptr<EngineCore::Scene> sceneWp,
                            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                            const auto& primitiveSp = sceneRendererSp->GetPrimitiveProxyByProxyId(sceneProxyId);
                            if (primitiveSp) {
                                primitiveSp->SetIsOutlineApplied(isOutlineApplied);
                            }
                        });
                    bIsOutlineStateDirty = false;
                }
            }
        }
    }
}
} // namespace EngineCore