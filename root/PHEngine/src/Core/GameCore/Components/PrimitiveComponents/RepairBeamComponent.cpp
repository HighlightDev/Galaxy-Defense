#include "RepairBeamComponent.h"

#include "Core/GameCore/Components/ProceduralBeamGeometry.h"

namespace EngineCore {

void RepairBeamComponent::OnRegistered()
{
    // A smooth wave needs more spine samples than the electric tube's default before the first bake.
    mLengthSegments = 64;
    Base::OnRegistered();
}

float RepairBeamComponent::GetWaveLoopPeriodSeconds() const
{
    return static_cast<float>(c_animationFramesCount) * mUpdateFrequency;
}

float RepairBeamComponent::GetWaveAmplitude() const
{
    return mJitterAmount;
}

void RepairBeamComponent::BakeBeamMesh(
    const float animationTime, std::vector<BeamVertex>& outVertices, std::vector<uint32_t>& outIndices) const
{
    const float radius = mBeamThickness * 0.1f; // thickness -> tube radius, as in DynamicBeamComponent

    // The base "jitter amount" is repurposed as the wave amplitude (in canonical perpendicular units = world units).
    ProceduralBeamGeometry::GenerateCanonicalAnimatedWaveBeamGeometry(
        radius,
        mRadialSegments,
        mLengthSegments,
        mJitterAmount,
        animationTime,
        GetWaveLoopPeriodSeconds(),
        c_waveSpatialCycles,
        c_waveTemporalCyclesPerLoop,
        outVertices,
        outIndices);
}

} // namespace EngineCore
