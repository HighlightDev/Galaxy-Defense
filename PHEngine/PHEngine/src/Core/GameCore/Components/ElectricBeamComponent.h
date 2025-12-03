#pragma once

#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/EngineObjectProperty.h"

#include <glm/vec3.hpp>

#include <memory>
#include <vector>

namespace EngineCore {
class RuntimeGeneratedLineComponent;
class StaticMeshComponent;
struct BeamVertex;
} // namespace EngineCore

namespace EngineCore {

enum class BeamRenderMode {
    ProceduralMesh, // Use procedural geometry (more realistic, volumetric)
    ProceduralElectric // Use procedural geometry with jittered segments
};

/**
 * Component for creating electric beam effects between two points
 * Supports multiple parallel beams with jitter animation
 */
class ElectricBeamComponent : public PrimitiveComponent {
    using Base = PrimitiveComponent;

private:
    // Mesh-based rendering
    std::vector<std::shared_ptr<StaticMeshComponent>> mBeamMeshes;

    glm::vec3 mStartPoint;
    glm::vec3 mEndPoint;
    glm::vec3 mBeamColor;
    float mBeamThickness;
    int mBeamCount;
    float mJitterAmount;
    float mUpdateFrequency;
    bool mIsActive;

    BeamRenderMode mRenderMode;
    int mRadialSegments;
    int mLengthSegments;
    float mAnimationTime;
    float mAnimationSpeed;

    float mTimeSinceLastUpdate;

public:
    ElectricBeamComponent(const std::string& gameObjectName);

    ~ElectricBeamComponent() override;

    void Tick(const float deltaTime) override;

    void Initialize() override;

    // Setters
    void SetStartPoint(const glm::vec3& point);
    void SetEndPoint(const glm::vec3& point);
    void SetBeamColor(const glm::vec3& color);
    void SetBeamThickness(float thickness);
    void SetBeamCount(int count);
    void SetJitterAmount(float amount);
    void SetUpdateFrequency(float frequency);
    void SetActive(bool active);
    void SetGeometrySegments(int radialSegments, int lengthSegments);
    void SetAnimationSpeed(float speed);
    void SetRenderMode(BeamRenderMode mode);

    // Getters
    glm::vec3 GetStartPoint() const;
    glm::vec3 GetEndPoint() const;
    glm::vec3 GetBeamColor() const;
    float GetBeamThickness() const;
    int GetBeamCount() const;
    float GetJitterAmount() const;
    float GetUpdateFrequency() const;
    bool IsActive() const;
    BeamRenderMode GetRenderMode() const;
    float GetAnimationSpeed() const;

private:
    void RegenerateBeams();
    void UpdateBeamPositions();
    glm::vec3 GetJitteredPoint(const glm::vec3& basePoint, float jitterScale) const;

    // Mesh-based methods
    void CreateBeamMeshes();
    void DestroyBeamMeshes();
    void UpdateBeamMesh(int beamIndex);
};

} // namespace EngineCore
