#pragma once

#include "Core/CommonCore/Timer.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"

#include <glm/vec3.hpp>

#include <memory>
#include <vector>

using namespace Graphics::Data;

namespace EngineCore {

enum class BeamRenderMode {
    ProceduralMesh, // Use procedural geometry (more realistic, volumetric)
    ProceduralElectric // Use procedural geometry with jittered segments
};

class StaticMeshComponent;
struct BeamVertex;

/**
 * Component for creating electric beam effects between two points
 * Supports multiple parallel beams with jitter animation
 */
class ElectricBeamComponent : public PrimitiveComponent {
    using Base = PrimitiveComponent;

private:
    constexpr static int32_t c_maxBeamsCount = 20;
    constexpr static int32_t c_minBeamsCount = 1;

    std::vector<std::tuple<std::vector<BeamVertex>, std::vector<uint32_t>>> mBeamMeshes;

    glm::vec3 mStartWorldPosition;
    glm::vec3 mEndWorldPosition;
    float mBeamThickness;
    int32_t mBeamCount;
    float mJitterAmount;
    float mUpdateFrequency;

    BeamRenderMode mRenderMode;
    int32_t mRadialSegments;
    int32_t mLengthSegments;
    float mAnimationTime;
    float mAnimationSpeed;

    float mTimeSinceLastUpdate;

    bool mIsRenderDataDirty{false};

    const MeshRenderData mRenderData;

public:
    ElectricBeamComponent(const std::string& gameObjectName, const MeshRenderData& renderData);

    ~ElectricBeamComponent() override;

    void Tick(const float deltaTime) override;

    void OnRegistered() override;

    void OnUnregistered() override;

    std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

    // Setters
    void SetStartWorldPosition(const glm::vec3& point);
    void SetEndWorldPosition(const glm::vec3& point);
    void SetBeamThickness(const float thickness);
    void SetBeamCount(const int32_t count);
    void SetJitterAmount(const float amount);
    void SetUpdateFrequency(const float frequency);
    void SetRadialSegments(const int32_t radialSegments);
    void SetAnimationSpeed(const float speed);
    void SetRenderMode(BeamRenderMode mode);

    // Getters
    glm::vec3 GetStartPoint() const;
    glm::vec3 GetEndPoint() const;
    float GetBeamThickness() const;
    int32_t GetBeamCount() const;
    float GetJitterAmount() const;
    float GetUpdateFrequency() const;
    BeamRenderMode GetRenderMode() const;
    float GetAnimationSpeed() const;
    int32_t GetRadialSegments() const;
    int32_t GetLengthSegments() const;

    const MeshRenderData& GetRenderData() const;

private:
    void SyncRenderData();
    void RegenerateBeams();
    glm::vec3 GetJitteredPoint(const glm::vec3& basePoint) const;

    // Mesh-based methods
    void CreateBeamMeshes();
    void DestroyBeamMeshes();
    void UpdateBeamMesh(const int32_t beamIndex);
};

} // namespace EngineCore
