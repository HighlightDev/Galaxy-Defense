#pragma once
#include "RuntimeGeneratedLineComponent.h"

#include <glm/vec3.hpp>

namespace EngineCore {
struct MeshComponentData;

// A flat XZ grid of lines whose points are pulled toward a world-space center (~1/dist), visualizing the gravity well
// of the singularity. Built procedurally on the CPU; reuses the runtime-generated-line infrastructure (line width is the
// width of each grid line). The grid is static once placed - the running-energy look comes from the material.
class GravityGridComponent : public RuntimeGeneratedLineComponent {
    using Base = RuntimeGeneratedLineComponent;

    glm::vec3 mGridCenterWorldSpacePosition{0.0f};

    float mGridHalfExtent{15.0f};

    int32_t mGridLineCount{11};

    int32_t mSegmentsPerLine{16};

    // Horizontal pull strength toward the center, and downward (Y) dip strength, both scaled by ~1/dist.
    float mDeformStrength{18.0f};

    float mDipStrength{7.0f};

    float mSoftening{2.0f};

public:
    GravityGridComponent(
        const std::shared_ptr<MeshComponentData>& meshComponentData,
        const MeshRenderData& renderData,
        const RuntimeGeneratedMeshPoolParameters& rtMeshParams);

    std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

    void SetGridCenterWorldSpacePosition(const glm::vec3& center);

    glm::vec3 GetGridCenterWorldSpacePosition() const;

    void SetGridHalfExtent(const float halfExtent);

    float GetGridHalfExtent() const;

    void SetGridLineCount(const int32_t lineCount);

    int32_t GetGridLineCount() const;

    void SetSegmentsPerLine(const int32_t segmentsPerLine);

    int32_t GetSegmentsPerLine() const;

    void SetDeformStrength(const float deformStrength);

    float GetDeformStrength() const;

    void SetDipStrength(const float dipStrength);

    float GetDipStrength() const;

    void SetSoftening(const float softening);

    float GetSoftening() const;

private:
    void SyncRenderData() override;
};

} // namespace EngineCore
