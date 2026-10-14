#pragma once
#include "RuntimeGeneratedLineComponent.h"

#include <glm/vec3.hpp>

#include <deque>
#include <memory>
#include <vector>

namespace EngineCore {
struct MeshComponentData;
class SceneComponent;

// A tapered plasma trail that follows a source component through the world. Each frame it samples the source's world
// position into a bounded history; the proxy builds a camera-facing ribbon through that history, wide and hot at the
// head (newest) and thinning toward the tail (oldest). Lives on an identity-transform host so the world-space history
// renders without a parent transform; gated by enabling/disabling the component.
class PlasmaTrailComponent : public RuntimeGeneratedLineComponent {
    using Base = RuntimeGeneratedLineComponent;

    std::weak_ptr<SceneComponent> mSourceComponentWp;

    std::deque<glm::vec3> mTrailPoints;

    size_t mMaxPoints{40};

    // Minimum world-space spacing between recorded points, so the trail records a smooth path rather than a jitter of
    // near-identical samples while the missile flies.
    float mMinPointSpacing{0.5f};

    // Whether new points are still being recorded. Set false on impact so the trail freezes as a path scar, then keeps
    // rendering (and fading via taper) until the component is disabled.
    bool mIsRecording{true};

    // Smooth fade-out: instead of popping off when disabled, the frozen trail tapers its width to zero over
    // mFadeOutDurationSec and then disables itself. mFadeFactor (1 -> 0) scales the width sent to the proxy.
    bool mIsFadingOut{false};
    float mFadeOutDurationSec{0.0f};
    float mFadeOutElapsedSec{0.0f};
    float mFadeFactor{1.0f};

public:
    PlasmaTrailComponent(
        const std::shared_ptr<MeshComponentData>& meshComponentData,
        const MeshRenderData& renderData,
        const RuntimeGeneratedMeshPoolParameters& rtMeshParams);

    std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void SetSourceComponent(const std::weak_ptr<SceneComponent>& sourceComponent);

    void SetMaxPoints(const size_t maxPoints);

    void SetMinPointSpacing(const float spacing);

    void SetIsRecording(const bool isRecording);

    // Freezes recording and begins a smooth width fade-out over durationSec; the component disables itself once the
    // fade completes, so the trail dissolves instead of vanishing instantly.
    void StartFadeOut(const float durationSec);

    // Drops the recorded path (e.g. when the pooled missile is respawned) so a reused trail doesn't show the old route.
    void ResetTrail();

    const std::vector<glm::vec3> GetTrailPointsSnapshot() const;

private:
    void SyncRenderData() override;
};

} // namespace EngineCore
