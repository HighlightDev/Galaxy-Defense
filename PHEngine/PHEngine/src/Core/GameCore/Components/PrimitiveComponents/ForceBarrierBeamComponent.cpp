#include "ForceBarrierBeamComponent.h"

#include "Core/GameCore/Components/ProceduralBeamGeometry.h"

namespace EngineCore {

void ForceBarrierBeamComponent::OnRegistered()
{
    // A smooth spiral needs more spine samples than the electric tube's default before the first bake.
    mLengthSegments = 80;
    Base::OnRegistered();
}

float ForceBarrierBeamComponent::GetSpiralLoopPeriodSeconds() const
{
    return static_cast<float>(c_animationFramesCount) * mUpdateFrequency;
}

float ForceBarrierBeamComponent::GetSpiralRadius() const
{
    return mJitterAmount;
}

void ForceBarrierBeamComponent::BakeBeamMesh(
    const float animationTime, std::vector<BeamVertex>& outVertices, std::vector<uint32_t>& outIndices) const
{
    const float radius = mBeamThickness * 0.1f; // thickness -> tube radius, as in DynamicBeamComponent

    // The base "jitter amount" is repurposed as the spiral radius (canonical perpendicular units = world units).
    ProceduralBeamGeometry::GenerateCanonicalAnimatedSpiralBeamGeometry(
        radius,
        mRadialSegments,
        mLengthSegments,
        mJitterAmount,
        animationTime,
        GetSpiralLoopPeriodSeconds(),
        c_spiralTurns,
        c_spiralTemporalCyclesPerLoop,
        outVertices,
        outIndices);
}

} // namespace EngineCore
