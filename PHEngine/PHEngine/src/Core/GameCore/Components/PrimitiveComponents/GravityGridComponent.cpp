#include "GravityGridComponent.h"

#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/GravityGridSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {
GravityGridComponent::GravityGridComponent(
    const std::shared_ptr<MeshComponentData>& meshComponentData,
    const MeshRenderData& renderData,
    const RuntimeGeneratedMeshPoolParameters& rtMeshParams)
    : RuntimeGeneratedLineComponent(meshComponentData, renderData, rtMeshParams)
{
}

std::shared_ptr<PrimitiveSceneProxy> GravityGridComponent::CreateSceneProxy() const
{
    return std::make_shared<Graphics::Proxy::GravityGridSceneProxy>(this);
}

void GravityGridComponent::SetGridCenterWorldSpacePosition(const glm::vec3& center)
{
    if (!EngineMath::CheckSimilarityVec3(center, mGridCenterWorldSpacePosition)) {
        mGridCenterWorldSpacePosition = center;
        mIsRenderDataDirty = true;
    }
}

glm::vec3 GravityGridComponent::GetGridCenterWorldSpacePosition() const
{
    return mGridCenterWorldSpacePosition;
}

void GravityGridComponent::SetGridHalfExtent(const float halfExtent)
{
    mGridHalfExtent = halfExtent;
}

float GravityGridComponent::GetGridHalfExtent() const
{
    return mGridHalfExtent;
}

void GravityGridComponent::SetGridLineCount(const int32_t lineCount)
{
    mGridLineCount = lineCount;
}

int32_t GravityGridComponent::GetGridLineCount() const
{
    return mGridLineCount;
}

void GravityGridComponent::SetSegmentsPerLine(const int32_t segmentsPerLine)
{
    mSegmentsPerLine = segmentsPerLine;
}

int32_t GravityGridComponent::GetSegmentsPerLine() const
{
    return mSegmentsPerLine;
}

void GravityGridComponent::SetDeformStrength(const float deformStrength)
{
    mDeformStrength = deformStrength;
}

float GravityGridComponent::GetDeformStrength() const
{
    return mDeformStrength;
}

void GravityGridComponent::SetDipStrength(const float dipStrength)
{
    mDipStrength = dipStrength;
}

float GravityGridComponent::GetDipStrength() const
{
    return mDipStrength;
}

void GravityGridComponent::SetSoftening(const float softening)
{
    mSoftening = softening;
}

float GravityGridComponent::GetSoftening() const
{
    return mSoftening;
}

void GravityGridComponent::SyncRenderData()
{
    static constexpr uint64_t functionId = Hash64_CT("GravityGridComponent::SyncRenderData");
    if (const auto& sceneSp = m_sceneWP.lock()) {
        if (const auto& sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                GetObjectId(),
                functionId,
                [sceneRenderer,
                 sceneProxyId = mSceneProxyId,
                 center = mGridCenterWorldSpacePosition,
                 halfExtent = mGridHalfExtent,
                 lineCount = mGridLineCount,
                 segmentsPerLine = mSegmentsPerLine,
                 deformStrength = mDeformStrength,
                 dipStrength = mDipStrength,
                 softening = mSoftening,
                 gridLineWidth = GetLineWidth()](
                    std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                    std::weak_ptr<EngineCore::Scene> sceneWp,
                    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                    if (const auto& gridProxySp = std::static_pointer_cast<Graphics::Proxy::GravityGridSceneProxy>(
                            sceneRenderer->GetPrimitiveProxyByProxyId(sceneProxyId))) {
                        gridProxySp->SetGridParams(
                            center, halfExtent, lineCount, segmentsPerLine, deformStrength, dipStrength, softening, gridLineWidth);
                    }
                });
        }
    }
}
} // namespace EngineCore
