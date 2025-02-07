#pragma once
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/RuntimeGeneratedMeshPoolParameters.h"
#include "RuntimeGeneratedLineComponent.h"

using namespace Resources;
using namespace Graphics::Data;
using namespace Graphics;

namespace EngineCore {
struct MeshComponentData;

class RuntimeGeneratedQuadraticBezierCurveComponent : public RuntimeGeneratedLineComponent {
    using Base = RuntimeGeneratedLineComponent;

    glm::vec3 mBezierControlPointWorldSpacePosition;

    int32_t mCurveSegmentsCount{1};

public:
    RuntimeGeneratedQuadraticBezierCurveComponent(
        const std::shared_ptr<MeshComponentData>& meshComponentData,
        const MeshRenderData& renderData,
        const RuntimeGeneratedMeshPoolParameters& rtMeshParams);

    std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

    void SetBezierControlPointWorldSpacePosition(const glm::vec3& position);

    glm::vec3 GetBezierControlPointWorldSpacePosition() const;

    void SetCurveSegmentsCount(const int32_t curveSegmentsCount);

    int32_t GetCurveSegmentsCount() const;

private:
    void SyncRenderData() override;
};

} // namespace EngineCore
