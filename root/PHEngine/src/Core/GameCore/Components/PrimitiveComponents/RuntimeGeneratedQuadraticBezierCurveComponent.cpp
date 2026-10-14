#include "RuntimeGeneratedQuadraticBezierCurveComponent.h"

#include "Core/GameCore/Components/ComponentData/MeshComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/SceneProxy/RuntimeGeneratedQuadraticBezierCurveSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

namespace EngineCore {
RuntimeGeneratedQuadraticBezierCurveComponent::RuntimeGeneratedQuadraticBezierCurveComponent(
    const std::shared_ptr<MeshComponentData>& meshComponentData,
    const MeshRenderData& renderData,
    const RuntimeGeneratedMeshPoolParameters& rtMeshParams)
    : RuntimeGeneratedLineComponent(meshComponentData, renderData, rtMeshParams)
{
}

std::shared_ptr<PrimitiveSceneProxy> RuntimeGeneratedQuadraticBezierCurveComponent::CreateSceneProxy() const
{
    return std::make_shared<RuntimeGeneratedQuadraticBezierCurveSceneProxy>(this);
}

void RuntimeGeneratedQuadraticBezierCurveComponent::SetBezierControlPointWorldSpacePosition(const glm::vec3& position)
{
    if (!EngineMath::CheckSimilarityVec3(position, mBezierControlPointWorldSpacePosition)) {
        mBezierControlPointWorldSpacePosition = position;
        mIsRenderDataDirty = true;
    }
}

glm::vec3 RuntimeGeneratedQuadraticBezierCurveComponent::GetBezierControlPointWorldSpacePosition() const
{
    return mBezierControlPointWorldSpacePosition;
}

void RuntimeGeneratedQuadraticBezierCurveComponent::SetCurveSegmentsCount(const int32_t curveSegmentsCount)
{
    if (mCurveSegmentsCount != curveSegmentsCount) {
        mCurveSegmentsCount = curveSegmentsCount;
        mIsRenderDataDirty = true;
    }
}

int32_t RuntimeGeneratedQuadraticBezierCurveComponent::GetCurveSegmentsCount() const
{
    return mCurveSegmentsCount;
}

void RuntimeGeneratedQuadraticBezierCurveComponent::SyncRenderData()
{
    static constexpr uint64_t functionId = Hash64_CT("RuntimeGeneratedQuadraticBezierCurveComponent::SyncRenderData");
    if (const auto& sceneSp = m_sceneWP.lock()) {
        if (const auto& sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                GetObjectId(),
                functionId,
                [sceneRenderer,
                 sceneProxyId = mSceneProxyId,
                 bezierControlPosition = mBezierControlPointWorldSpacePosition,
                 curveSegmentsCount = mCurveSegmentsCount,
                 lineBeginWorldSpacePosition = GetLineBeginWorldSpacePosition(),
                 lineEndWorldSpacePosition = GetLineEndWorldSpacePosition(),
                 lineWidth = GetLineWidth()](
                    std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                    std::weak_ptr<EngineCore::Scene> sceneWp,
                    std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                    if (const auto& lineProxySp = std::static_pointer_cast<RuntimeGeneratedQuadraticBezierCurveSceneProxy>(
                            sceneRenderer->GetPrimitiveProxyByProxyId(sceneProxyId))) {
                        lineProxySp->SetBezierControlPointWorldSpacePosition(bezierControlPosition);
                        lineProxySp->SetCurveSegmentsCount(curveSegmentsCount);
                        lineProxySp->SetLineBeginWorldSpacePosition(lineBeginWorldSpacePosition);
                        lineProxySp->SetLineEndWorldSpacePosition(lineEndWorldSpacePosition);
                        lineProxySp->SetLineWidth(lineWidth);
                    }
                });
        }
    }
}
} // namespace EngineCore
