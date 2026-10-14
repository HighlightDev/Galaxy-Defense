#include "OrbitalRingComponent.h"

#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/OrbitalRingSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {
OrbitalRingComponent::OrbitalRingComponent(
    const std::shared_ptr<MeshComponentData>& meshComponentData,
    const MeshRenderData& renderData,
    const RuntimeGeneratedMeshPoolParameters& rtMeshParams)
    : RuntimeGeneratedLineComponent(meshComponentData, renderData, rtMeshParams)
{
}

std::shared_ptr<PrimitiveSceneProxy> OrbitalRingComponent::CreateSceneProxy() const
{
    return std::make_shared<Graphics::Proxy::OrbitalRingSceneProxy>(this);
}

void OrbitalRingComponent::UnpausableTick(const float deltaTimeSec, const float playSpeed)
{
    mRingSpinAngleDeg += mRingSpinSpeedDegPerSec * deltaTimeSec;
    if (mRingSpinAngleDeg >= 360.0f) {
        mRingSpinAngleDeg -= 360.0f;
    } else if (mRingSpinAngleDeg < 0.0f) {
        mRingSpinAngleDeg += 360.0f;
    }

    // Continuously re-push so the ring keeps tumbling; Base::UnpausableTick performs the actual sync when dirty.
    mIsRenderDataDirty = true;
    Base::UnpausableTick(deltaTimeSec, playSpeed);
}

void OrbitalRingComponent::SetRingCenterWorldSpacePosition(const glm::vec3& center)
{
    mRingCenterWorldSpacePosition = center;
    mIsRenderDataDirty = true;
}

glm::vec3 OrbitalRingComponent::GetRingCenterWorldSpacePosition() const
{
    return mRingCenterWorldSpacePosition;
}

void OrbitalRingComponent::SetRingRadius(const float radius)
{
    mRingRadius = radius;
}

float OrbitalRingComponent::GetRingRadius() const
{
    return mRingRadius;
}

void OrbitalRingComponent::SetRingTumbleAxis(const glm::vec3& axis)
{
    mRingTumbleAxis = axis;
}

glm::vec3 OrbitalRingComponent::GetRingTumbleAxis() const
{
    return mRingTumbleAxis;
}

void OrbitalRingComponent::SetRingSpinSpeedDegPerSec(const float degPerSec)
{
    mRingSpinSpeedDegPerSec = degPerSec;
}

void OrbitalRingComponent::SetRingSegmentsCount(const int32_t count)
{
    mRingSegmentsCount = count;
}

int32_t OrbitalRingComponent::GetRingSegmentsCount() const
{
    return mRingSegmentsCount;
}

float OrbitalRingComponent::GetRingSpinAngleDeg() const
{
    return mRingSpinAngleDeg;
}

void OrbitalRingComponent::SyncRenderData()
{
    static constexpr uint64_t functionId = Hash64_CT("OrbitalRingComponent::SyncRenderData");
    if (const auto& sceneSp = m_sceneWP.lock()) {
        if (const auto& sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                GetObjectId(),
                functionId,
                [sceneRenderer,
                 sceneProxyId = mSceneProxyId,
                 center = mRingCenterWorldSpacePosition,
                 radius = mRingRadius,
                 tumbleAxis = mRingTumbleAxis,
                 spinAngleDeg = mRingSpinAngleDeg,
                 segmentsCount = mRingSegmentsCount,
                 ringWidth = GetLineWidth()](
                    std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                    std::weak_ptr<EngineCore::Scene> sceneWp,
                    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                    if (const auto& ringProxySp = std::static_pointer_cast<Graphics::Proxy::OrbitalRingSceneProxy>(
                            sceneRenderer->GetPrimitiveProxyByProxyId(sceneProxyId))) {
                        ringProxySp->SetRingParams(center, radius, tumbleAxis, spinAngleDeg, segmentsCount, ringWidth);
                    }
                });
        }
    }
}
} // namespace EngineCore
