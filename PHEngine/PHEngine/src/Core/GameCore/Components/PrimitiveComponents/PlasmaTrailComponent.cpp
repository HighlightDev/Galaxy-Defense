#include "PlasmaTrailComponent.h"

#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/PlasmaTrailSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/common.hpp>
#include <glm/geometric.hpp>

namespace EngineCore {
PlasmaTrailComponent::PlasmaTrailComponent(
    const std::shared_ptr<MeshComponentData>& meshComponentData,
    const MeshRenderData& renderData,
    const RuntimeGeneratedMeshPoolParameters& rtMeshParams)
    : RuntimeGeneratedLineComponent(meshComponentData, renderData, rtMeshParams)
{
}

std::shared_ptr<PrimitiveSceneProxy> PlasmaTrailComponent::CreateSceneProxy() const
{
    return std::make_shared<Graphics::Proxy::PlasmaTrailSceneProxy>(this);
}

void PlasmaTrailComponent::Tick(const float deltaTimeSec, const float playSpeed)
{
    if (mIsRecording) {
        if (const auto& sourceSp = mSourceComponentWp.lock()) {
            const glm::vec3 headPosition = sourceSp->GetHierarchyAccumulatedTranslation();
            if (mTrailPoints.empty() || glm::length(headPosition - mTrailPoints.back()) >= mMinPointSpacing) {
                mTrailPoints.push_back(headPosition);
                while (mTrailPoints.size() > mMaxPoints) {
                    mTrailPoints.pop_front();
                }
            }
        }
    }

    if (mIsFadingOut) {
        mFadeOutElapsedSec += deltaTimeSec;
        mFadeFactor = mFadeOutDurationSec > 0.0f
            ? glm::clamp(1.0f - mFadeOutElapsedSec / mFadeOutDurationSec, 0.0f, 1.0f)
            : 0.0f;
        if (mFadeOutElapsedSec >= mFadeOutDurationSec) {
            // Fully tapered away: switch off so the proxy stops rendering (it is already invisible, so no pop).
            mIsFadingOut = false;
            mFadeFactor = 0.0f;
            SetIsEnabled(false);
        }
    }

    // Re-push every frame: the ribbon is camera-facing, so it must be rebuilt as the camera moves even if the path
    // itself did not change. Base::Tick performs the actual sync when dirty. Using Tick (not UnpausableTick) means the
    // component only updates while it is enabled — it sits idle in the pool and between launches instead of forever.
    mIsRenderDataDirty = true;
    Base::Tick(deltaTimeSec, playSpeed);
}

void PlasmaTrailComponent::SetSourceComponent(const std::weak_ptr<SceneComponent>& sourceComponent)
{
    mSourceComponentWp = sourceComponent;
}

void PlasmaTrailComponent::SetMaxPoints(const size_t maxPoints)
{
    mMaxPoints = maxPoints < 2 ? 2 : maxPoints;
}

void PlasmaTrailComponent::SetMinPointSpacing(const float spacing)
{
    mMinPointSpacing = spacing;
}

void PlasmaTrailComponent::SetIsRecording(const bool isRecording)
{
    mIsRecording = isRecording;
}

void PlasmaTrailComponent::StartFadeOut(const float durationSec)
{
    mIsRecording = false;
    mIsFadingOut = true;
    mFadeOutDurationSec = durationSec;
    mFadeOutElapsedSec = 0.0f;
    mFadeFactor = 1.0f;
}

void PlasmaTrailComponent::ResetTrail()
{
    mTrailPoints.clear();
    mIsRecording = true;
    mIsFadingOut = false;
    mFadeOutElapsedSec = 0.0f;
    mFadeFactor = 1.0f;
    mIsRenderDataDirty = true;
}

const std::vector<glm::vec3> PlasmaTrailComponent::GetTrailPointsSnapshot() const
{
    return std::vector<glm::vec3>(mTrailPoints.cbegin(), mTrailPoints.cend());
}

void PlasmaTrailComponent::SyncRenderData()
{
    static constexpr uint64_t functionId = Hash64_CT("PlasmaTrailComponent::SyncRenderData");
    if (const auto& sceneSp = m_sceneWP.lock()) {
        if (const auto& sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                GetObjectId(),
                functionId,
                [sceneRenderer,
                 sceneProxyId = mSceneProxyId,
                 trailPoints = GetTrailPointsSnapshot(),
                 width = GetLineWidth() * mFadeFactor](
                    std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                    std::weak_ptr<EngineCore::Scene> sceneWp,
                    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                    if (const auto& trailProxySp = std::static_pointer_cast<Graphics::Proxy::PlasmaTrailSceneProxy>(
                            sceneRenderer->GetPrimitiveProxyByProxyId(sceneProxyId))) {
                        trailProxySp->SetTrailParams(trailPoints, width);
                    }
                });
        }
    }
}
} // namespace EngineCore
