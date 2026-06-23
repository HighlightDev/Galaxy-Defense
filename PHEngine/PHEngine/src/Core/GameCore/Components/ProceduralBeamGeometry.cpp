#include "ProceduralBeamGeometry.h"

#include <glm/gtx/quaternion.hpp>

#include <cmath>
#include <random>

namespace EngineCore {

glm::vec3 ProceduralBeamGeometry::GetJitteredPoint(const glm::vec3& point, float jitterAmount)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

    return point + glm::vec3(dis(gen) * jitterAmount, dis(gen) * jitterAmount, dis(gen) * jitterAmount);
}

glm::vec3
ProceduralBeamGeometry::GetAnimatedJitteredPoint(const glm::vec3& point, float jitterAmount, float time, float speed, int seed)
{
    // Use Perlin-like smooth noise for organic animation
    const float timeScaled = time * speed;
    const float seedOffset = static_cast<float>(seed) * 10.0f;

    const float noiseX = SmoothNoise(point.x + timeScaled + seedOffset, point.y, point.z);
    const float noiseY = SmoothNoise(point.x, point.y + timeScaled + seedOffset + 100.0f, point.z);
    const float noiseZ = SmoothNoise(point.x, point.y, point.z + timeScaled + seedOffset + 200.0f);

    return point
        + glm::vec3(
               (noiseX * 2.0f - 1.0f) * jitterAmount,
               (noiseY * 2.0f - 1.0f) * jitterAmount,
               (noiseZ * 2.0f - 1.0f) * jitterAmount);
}

float ProceduralBeamGeometry::SmoothNoise(float x, float y, float z)
{
    // Simple 3D smooth noise function using sine waves
    // This creates smooth, continuous animation
    const float freq1 = 1.0f;
    const float freq2 = 2.3f;
    const float freq3 = 3.7f;

    float noise = 0.0f;
    noise += std::sin(x * freq1 + y * freq2) * 0.5f;
    noise += std::sin(y * freq2 + z * freq1) * 0.3f;
    noise += std::sin(z * freq3 + x * freq2) * 0.2f;

    // Normalize to [0, 1]
    return (noise + 1.0f) * 0.5f;
}

void ProceduralBeamGeometry::GenerateCanonicalAnimatedElectricBeamGeometry(
    float radius,
    int radialSegments,
    int lengthSegments,
    float jitterAmount,
    float animationTime,
    float animationSpeed,
    std::vector<BeamVertex>& outVertices,
    std::vector<uint32_t>& outIndices)
{
    outVertices.clear();
    outIndices.clear();

    if (radialSegments < 3)
        radialSegments = 3;
    if (lengthSegments < 2)
        lengthSegments = 2;

    // Canonical spine runs along +Z from 0 to 1. Jitter perturbs only the perpendicular XY plane so the
    // result stays length-independent: the proxy scales Z by the real beam length, and scaling baked-in
    // longitudinal jitter would distort it differently per length.
    std::vector<glm::vec3> spinePoints;
    spinePoints.reserve(lengthSegments + 1);

    for (int seg = 0; seg <= lengthSegments; ++seg) {
        const float t = static_cast<float>(seg) / lengthSegments;
        glm::vec3 point(0.0f, 0.0f, t);

        if (seg > 0 && seg < lengthSegments) {
            const glm::vec3 jittered = GetAnimatedJitteredPoint(point, jitterAmount, animationTime, animationSpeed, seg);
            point.x = jittered.x; // perpendicular only — keep the longitudinal (Z) position intact
            point.y = jittered.y;
        }

        spinePoints.push_back(point);
    }

    // Rings are oriented to the fixed canonical axis (+Z), NOT to the local jittered spine direction.
    const glm::vec3 segTangent(1.0f, 0.0f, 0.0f);
    const glm::vec3 segBitangent(0.0f, 1.0f, 0.0f);

    for (int seg = 0; seg <= lengthSegments; ++seg) {
        const glm::vec3& center = spinePoints[seg];
        const float vCoord = static_cast<float>(seg) / lengthSegments;

        const float pulsePhase = animationTime * animationSpeed * 2.0f + seg * 0.5f;
        const float radiusVariation = 1.0f + (std::sin(pulsePhase) * 0.15f);
        const float segmentRadius = radius * radiusVariation;

        for (int i = 0; i <= radialSegments; ++i) {
            const float angle = (static_cast<float>(i) / radialSegments) * 2.0f * glm::pi<float>();
            const float cosAngle = std::cos(angle);
            const float sinAngle = std::sin(angle);

            const glm::vec3 offset = (segTangent * cosAngle + segBitangent * sinAngle) * segmentRadius;
            const glm::vec3 position = center + offset;
            const glm::vec3 normal = glm::normalize(offset);
            const glm::vec2 texCoord(static_cast<float>(i) / radialSegments, vCoord);

            outVertices.push_back({position, normal, texCoord});
        }
    }

    const int vertsPerRing = radialSegments + 1;
    for (int seg = 0; seg < lengthSegments; ++seg) {
        for (int i = 0; i < radialSegments; ++i) {
            const uint32_t topLeft = seg * vertsPerRing + i;
            const uint32_t topRight = seg * vertsPerRing + i + 1;
            const uint32_t bottomLeft = (seg + 1) * vertsPerRing + i;
            const uint32_t bottomRight = (seg + 1) * vertsPerRing + i + 1;

            outIndices.push_back(topLeft);
            outIndices.push_back(bottomLeft);
            outIndices.push_back(topRight);

            outIndices.push_back(topRight);
            outIndices.push_back(bottomLeft);
            outIndices.push_back(bottomRight);
        }
    }
}

void ProceduralBeamGeometry::GenerateCanonicalAnimatedWaveBeamGeometry(
    float radius,
    int radialSegments,
    int lengthSegments,
    float waveAmplitude,
    float animationTime,
    float loopPeriod,
    float spatialCycles,
    float temporalCyclesPerLoop,
    std::vector<BeamVertex>& outVertices,
    std::vector<uint32_t>& outIndices)
{
    outVertices.clear();
    outIndices.clear();

    if (radialSegments < 3)
        radialSegments = 3;
    if (lengthSegments < 2)
        lengthSegments = 2;

    const float twoPi = 2.0f * glm::pi<float>();
    // Time term normalised to the bake loop: advances temporalCyclesPerLoop full cycles over loopPeriod, so frame N
    // (animationTime == loopPeriod) lands a whole number of cycles past frame 0 and the cycled frame set has no seam.
    const float loopPhase = (loopPeriod > 0.0001f) ? (animationTime / loopPeriod) * twoPi * temporalCyclesPerLoop : 0.0f;

    std::vector<glm::vec3> spinePoints;
    spinePoints.reserve(lengthSegments + 1);

    for (int seg = 0; seg <= lengthSegments; ++seg) {
        const float t = static_cast<float>(seg) / lengthSegments;
        const float window = std::sin(t * glm::pi<float>()); // 0 at both ends → the beam meets its endpoints cleanly
        const float phase = t * spatialCycles * twoPi - loopPhase;
        const float displacement = std::sin(phase) * waveAmplitude * window;
        spinePoints.emplace_back(displacement, 0.0f, t);
    }

    // Rings are oriented to the fixed canonical axis (+Z), so the tube cross-section sweeps around the wavy spine.
    const glm::vec3 segTangent(1.0f, 0.0f, 0.0f);
    const glm::vec3 segBitangent(0.0f, 1.0f, 0.0f);

    for (int seg = 0; seg <= lengthSegments; ++seg) {
        const glm::vec3& center = spinePoints[seg];
        const float vCoord = static_cast<float>(seg) / lengthSegments;

        const float pulse = 1.0f + std::sin(loopPhase + seg * 0.3f) * 0.1f; // gentle "breathing"
        const float segmentRadius = radius * pulse;

        for (int i = 0; i <= radialSegments; ++i) {
            const float angle = (static_cast<float>(i) / radialSegments) * twoPi;
            const glm::vec3 offset = (segTangent * std::cos(angle) + segBitangent * std::sin(angle)) * segmentRadius;
            const glm::vec3 position = center + offset;
            const glm::vec3 normal = glm::normalize(offset);
            const glm::vec2 texCoord(static_cast<float>(i) / radialSegments, vCoord);

            outVertices.push_back({position, normal, texCoord});
        }
    }

    const int vertsPerRing = radialSegments + 1;
    for (int seg = 0; seg < lengthSegments; ++seg) {
        for (int i = 0; i < radialSegments; ++i) {
            const uint32_t topLeft = seg * vertsPerRing + i;
            const uint32_t topRight = seg * vertsPerRing + i + 1;
            const uint32_t bottomLeft = (seg + 1) * vertsPerRing + i;
            const uint32_t bottomRight = (seg + 1) * vertsPerRing + i + 1;

            outIndices.push_back(topLeft);
            outIndices.push_back(bottomLeft);
            outIndices.push_back(topRight);

            outIndices.push_back(topRight);
            outIndices.push_back(bottomLeft);
            outIndices.push_back(bottomRight);
        }
    }
}

} // namespace EngineCore
