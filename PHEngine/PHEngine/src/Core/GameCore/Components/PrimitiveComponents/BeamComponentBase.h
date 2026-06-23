#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/RuntimeGeneratedMeshPoolParameters.h"

#include <glm/vec3.hpp>

#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>

using namespace Graphics::Data;
using namespace Resources;

namespace EngineCore {

struct BeamVertex;

/**
 * Shared base for beam components. Bakes a fixed set of animation frames once and ships them to the
 * (shared) ElectricBeamSceneProxy, which cycles through them on the render thread — there is no per-tick
 * jitter regeneration. Subclasses choose the geometry/placement strategy via the virtual hooks below;
 * the only concrete subclass is DynamicBeamComponent (canonical geometry placed by a per-tick world
 * matrix, so endpoints can move freely without re-baking).
 */
class BeamComponentBase : public PrimitiveComponent {
    using Base = PrimitiveComponent;

protected:
    constexpr static int32_t c_maxBeamsCount = 20;
    constexpr static int32_t c_minBeamsCount = 1;

    // Number of pre-baked animation frames the proxy cycles through. Trades GPU memory for loop length:
    // at the default update frequency (20 Hz) this is a ~2.4 s loop. Tunable.
    constexpr static int32_t c_animationFramesCount = 48;

    RuntimeGeneratedMeshPoolParameters mBeamMeshPoolParams;

    // Pre-baked animation frames. Outer vector: one entry per animation frame. Inner vector: one mesh per
    // beam. Baked on the game thread and shipped to the proxy, which then cycles through them on its own.
    std::vector<std::vector<std::tuple<std::vector<BeamVertex>, std::vector<uint32_t>>>> mAnimationFrames;

    glm::vec3 mStartWorldPosition;
    glm::vec3 mEndWorldPosition;
    float mBeamThickness;
    int32_t mBeamCount;
    float mJitterAmount;
    float mUpdateFrequency;

    int32_t mRadialSegments;
    int32_t mLengthSegments;
    float mAnimationSpeed;

    // A geometry param (or, for Static, an endpoint) changed → re-bake + resend the whole frame set.
    bool mAreFramesDirty{false};

    const MeshRenderData mRenderData;

public:
    BeamComponentBase(
        const std::string& gameObjectName,
        const MeshRenderData& renderData,
        const RuntimeGeneratedMeshPoolParameters& beamMeshPoolParams,
        const bool isEnabled = true,
        const bool isVisible = true);

    ~BeamComponentBase() override;

    void Tick(const float deltaTime, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override;

    void OnRegistered() override;

    void OnUnregistered() override;

    std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

    std::shared_ptr<Scripts::LuaProxy> ReplicateLuaProxy() override;

    // Setters
    void SetStartWorldPosition(const glm::vec3& point);
    void SetEndWorldPosition(const glm::vec3& point);
    void SetBeamThickness(const float thickness);
    void SetBeamCount(const int32_t count);
    void SetJitterAmount(const float amount);
    void SetUpdateFrequency(const float frequency);
    void SetRadialSegments(const int32_t radialSegments);
    void SetAnimationSpeed(const float speed);

    // Getters
    RuntimeGeneratedMeshPoolParameters GetRuntimeGeneratedMeshPoolParameters() const;
    glm::vec3 GetStartPoint() const;
    glm::vec3 GetEndPoint() const;
    float GetBeamThickness() const;
    int32_t GetBeamCount() const;
    float GetJitterAmount() const;
    float GetUpdateFrequency() const;
    float GetAnimationSpeed() const;
    int32_t GetRadialSegments() const;
    int32_t GetLengthSegments() const;

    const MeshRenderData& GetRenderData() const;

protected:
    // Bake one beam mesh for the given animation time (frame time + per-beam offset). Strategy hook:
    // Static bakes in world space from the endpoints; Dynamic bakes the canonical unit beam.
    virtual void BakeBeamMesh(
        const float animationTime, std::vector<BeamVertex>& outVertices, std::vector<uint32_t>& outIndices) const = 0;

    // Whether the current state can produce geometry (Static: non-degenerate endpoints; Dynamic: always).
    virtual bool CanBakeGeometry() const { return true; }

    // Called whenever an endpoint changes. Default (Static): re-bake. Dynamic overrides to resend the matrix.
    virtual void OnEndpointsChanged() { mAreFramesDirty = true; }

    // Called from UnpausableTick — i.e. after the owning actor's Tick has set the endpoints and BEFORE
    // PrimitiveComponent::UnpausableTick enqueues the enable job, so the proxy never renders enabled with
    // a stale placement. Dynamic ships its world matrix here; Static does nothing.
    virtual void SyncTransformIfDirty() {}

    void GenerateAnimationFrames();
    void SyncAnimationFrames();
};

} // namespace EngineCore
