#pragma once
#include "RuntimeGeneratedLineComponent.h"

#include <glm/vec3.hpp>

namespace EngineCore {
struct MeshComponentData;

// A flat ring (annular band) generated procedurally around a world-space center, tumbling about an in-plane axis to
// produce an orbital look. Reuses the runtime-generated-line infrastructure (line width is used as the band width).
class OrbitalRingComponent : public RuntimeGeneratedLineComponent {
    using Base = RuntimeGeneratedLineComponent;

    glm::vec3 mRingCenterWorldSpacePosition{0.0f};

    float mRingRadius{5.0f};

    // Unit axis (in the ring plane) the ring tumbles about; differs per ring so the two rings orbit in different planes.
    glm::vec3 mRingTumbleAxis{1.0f, 0.0f, 0.0f};

    float mRingSpinSpeedDegPerSec{90.0f};

    float mRingSpinAngleDeg{0.0f};

    int32_t mRingSegmentsCount{48};

public:
    OrbitalRingComponent(
        const std::shared_ptr<MeshComponentData>& meshComponentData,
        const MeshRenderData& renderData,
        const RuntimeGeneratedMeshPoolParameters& rtMeshParams);

    std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override;

    void SetRingCenterWorldSpacePosition(const glm::vec3& center);

    glm::vec3 GetRingCenterWorldSpacePosition() const;

    void SetRingRadius(const float radius);

    float GetRingRadius() const;

    void SetRingTumbleAxis(const glm::vec3& axis);

    glm::vec3 GetRingTumbleAxis() const;

    void SetRingSpinSpeedDegPerSec(const float degPerSec);

    void SetRingSegmentsCount(const int32_t count);

    int32_t GetRingSegmentsCount() const;

    float GetRingSpinAngleDeg() const;

private:
    void SyncRenderData() override;
};

} // namespace EngineCore
