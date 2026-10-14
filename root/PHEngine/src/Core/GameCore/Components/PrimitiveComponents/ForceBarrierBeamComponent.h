#pragma once
#include "DynamicBeamComponent.h"

#include <cstdint>
#include <vector>

namespace EngineCore {
struct BeamVertex;

// A corkscrewing force-barrier beam built on the baked-frame beam pipeline (like the repair beam / electro ray). Only
// the geometry strategy differs from DynamicBeamComponent: instead of electric jitter / a sine wave, the canonical
// tube's spine winds around the axis as a spiral toward the target. Repurposes the base "jitter amount" as the spiral
// radius.
class ForceBarrierBeamComponent : public DynamicBeamComponent {
    using Base = DynamicBeamComponent;

public:
    // Spiral shape: how many full turns are visible along the beam at once, and whole turns of phase played per baked
    // loop (must be integer for a seamless loop).
    static constexpr float c_spiralTurns = 3.0f;
    static constexpr float c_spiralTemporalCyclesPerLoop = 1.0f;

    using DynamicBeamComponent::DynamicBeamComponent; // inherit constructors

    void OnRegistered() override;

    // The bake loop length in seconds (frame count x frame step); the spiral phase is normalised to this.
    float GetSpiralLoopPeriodSeconds() const;

    // The spiral radius (the base "jitter amount" is repurposed for it).
    float GetSpiralRadius() const;

protected:
    void BakeBeamMesh(
        const float animationTime, std::vector<BeamVertex>& outVertices, std::vector<uint32_t>& outIndices) const override;
};

} // namespace EngineCore
