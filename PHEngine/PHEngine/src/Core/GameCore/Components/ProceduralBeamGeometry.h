#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace EngineCore {

struct BeamVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;
};

class ProceduralBeamGeometry {
public:
    ProceduralBeamGeometry() = default;
    ~ProceduralBeamGeometry() = default;

    // Generate a canonical (local-space) animated beam running along +Z from (0,0,0) to (0,0,1), used by
    // DynamicBeamComponent. The proxy maps this unit beam onto the live world-space [start,end] segment
    // via a world matrix, so the frames can be baked once and reused while the endpoints move. Jitter is
    // applied only in the perpendicular XY plane (length-independent: not stretched by the proxy's Z scale).
    static void GenerateCanonicalAnimatedElectricBeamGeometry(
        float radius,
        int radialSegments,
        int lengthSegments,
        float jitterAmount,
        float animationTime,
        float animationSpeed,
        std::vector<BeamVertex>& outVertices,
        std::vector<uint32_t>& outIndices);

    // Canonical (local-space) animated repair beam: a tube along +Z (0..1) whose spine is offset by a travelling sine
    // wave in the perpendicular X plane (anchored to 0 at both ends so it meets its endpoints). Like the electric
    // version, the wave only perturbs the perpendicular plane so the proxy's Z scale keeps it length-independent. The
    // wave is normalised to loopPeriod so a fixed frame set cycles seamlessly: across one loop it advances exactly
    // temporalCyclesPerLoop full cycles, with spatialCycles ripples visible along the beam at any instant.
    static void GenerateCanonicalAnimatedWaveBeamGeometry(
        float radius,
        int radialSegments,
        int lengthSegments,
        float waveAmplitude,
        float animationTime,
        float loopPeriod,
        float spatialCycles,
        float temporalCyclesPerLoop,
        std::vector<BeamVertex>& outVertices,
        std::vector<uint32_t>& outIndices);

private:
    static glm::vec3 GetJitteredPoint(const glm::vec3& point, float jitterAmount);
    static glm::vec3 GetAnimatedJitteredPoint(const glm::vec3& point, float jitterAmount, float time, float speed, int seed);
    static float SmoothNoise(float x, float y, float z);
};

} // namespace EngineCore
