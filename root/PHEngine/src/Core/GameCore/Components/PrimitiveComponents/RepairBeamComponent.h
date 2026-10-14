#pragma once
#include "DynamicBeamComponent.h"

#include <cstdint>
#include <vector>

namespace EngineCore {
struct BeamVertex;

class RepairBeamComponent : public DynamicBeamComponent {
    using Base = DynamicBeamComponent;

public:
    // Wave shape, shared by the baked geometry and anything that must ride the wave (e.g. the nanobots): spatial ripples
    // visible along the beam at once, and whole wave cycles played per baked loop (must be integer for a seamless loop).
    static constexpr float c_waveSpatialCycles = 1.5f;
    static constexpr float c_waveTemporalCyclesPerLoop = 2.0f;

    using DynamicBeamComponent::DynamicBeamComponent; // inherit constructors

    void OnRegistered() override;

    // The bake loop length in seconds (frame count x frame step); the wave time term is normalised to this.
    float GetWaveLoopPeriodSeconds() const;

    // The wave amplitude (the base "jitter amount" is repurposed for it).
    float GetWaveAmplitude() const;

protected:
    void BakeBeamMesh(
        const float animationTime, std::vector<BeamVertex>& outVertices, std::vector<uint32_t>& outIndices) const override;
};

} // namespace EngineCore
