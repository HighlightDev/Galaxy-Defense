#include "ProceduralBeamGeometry.h"

#include <glm/gtx/quaternion.hpp>

#include <cmath>
#include <random>

namespace EngineCore {

void ProceduralBeamGeometry::GenerateBeamGeometry(
    const glm::vec3& startPoint,
    const glm::vec3& endPoint,
    float radius,
    int segments,
    std::vector<BeamVertex>& outVertices,
    std::vector<uint32_t>& outIndices)
{
    outVertices.clear();
    outIndices.clear();

    if (segments < 3)
        segments = 3;

    const glm::vec3 direction = glm::normalize(endPoint - startPoint);
    glm::vec3 tangent, bitangent;
    CalculateTangentBasis(direction, tangent, bitangent);

    // Generate vertices for both caps
    for (int cap = 0; cap < 2; ++cap) {
        const glm::vec3 center = (cap == 0) ? startPoint : endPoint;
        const float vCoord = static_cast<float>(cap);

        for (int i = 0; i <= segments; ++i) {
            const float angle = (static_cast<float>(i) / segments) * 2.0f * glm::pi<float>();
            const float cosAngle = std::cos(angle);
            const float sinAngle = std::sin(angle);

            const glm::vec3 offset = (tangent * cosAngle + bitangent * sinAngle) * radius;
            const glm::vec3 position = center + offset;
            const glm::vec3 normal = glm::normalize(offset);
            const glm::vec2 texCoord(static_cast<float>(i) / segments, vCoord);

            outVertices.push_back({position, normal, texCoord});
        }
    }

    // Generate indices
    const int vertsPerCap = segments + 1;
    for (int i = 0; i < segments; ++i) {
        const uint32_t topLeft = i;
        const uint32_t topRight = i + 1;
        const uint32_t bottomLeft = vertsPerCap + i;
        const uint32_t bottomRight = vertsPerCap + i + 1;

        // First triangle
        outIndices.push_back(topLeft);
        outIndices.push_back(bottomLeft);
        outIndices.push_back(topRight);

        // Second triangle
        outIndices.push_back(topRight);
        outIndices.push_back(bottomLeft);
        outIndices.push_back(bottomRight);
    }
}

void ProceduralBeamGeometry::GenerateElectricBeamGeometry(
    const glm::vec3& startPoint,
    const glm::vec3& endPoint,
    float radius,
    int radialSegments,
    int lengthSegments,
    float jitterAmount,
    std::vector<BeamVertex>& outVertices,
    std::vector<uint32_t>& outIndices)
{
    outVertices.clear();
    outIndices.clear();

    if (radialSegments < 3)
        radialSegments = 3;
    if (lengthSegments < 2)
        lengthSegments = 2;

    const glm::vec3 baseDirection = endPoint - startPoint;
    const float totalLength = glm::length(baseDirection);
    const glm::vec3 direction = baseDirection / totalLength;

    glm::vec3 tangent, bitangent;
    CalculateTangentBasis(direction, tangent, bitangent);

    // Generate spine points with jitter
    std::vector<glm::vec3> spinePoints;
    spinePoints.reserve(lengthSegments + 1);

    for (int seg = 0; seg <= lengthSegments; ++seg) {
        const float t = static_cast<float>(seg) / lengthSegments;
        glm::vec3 point = glm::mix(startPoint, endPoint, t);

        // Add jitter to intermediate points (not start/end)
        if (seg > 0 && seg < lengthSegments) {
            point = GetJitteredPoint(point, jitterAmount);
        }

        spinePoints.push_back(point);
    }

    // Generate vertices around each spine point
    for (int seg = 0; seg <= lengthSegments; ++seg) {
        const glm::vec3& center = spinePoints[seg];
        const float vCoord = static_cast<float>(seg) / lengthSegments;

        // Recalculate tangent basis for each segment to follow jittered spine
        glm::vec3 segmentDirection;
        if (seg == 0) {
            segmentDirection = glm::normalize(spinePoints[1] - spinePoints[0]);
        } else if (seg == lengthSegments) {
            segmentDirection = glm::normalize(spinePoints[lengthSegments] - spinePoints[lengthSegments - 1]);
        } else {
            segmentDirection = glm::normalize(spinePoints[seg + 1] - spinePoints[seg - 1]);
        }

        glm::vec3 segTangent, segBitangent;
        CalculateTangentBasis(segmentDirection, segTangent, segBitangent);

        // Add slight radius variation for more organic look
        const float radiusVariation = 1.0f + (std::sin(seg * 2.5f) * 0.15f);
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

    // Generate indices
    const int vertsPerRing = radialSegments + 1;
    for (int seg = 0; seg < lengthSegments; ++seg) {
        for (int i = 0; i < radialSegments; ++i) {
            const uint32_t topLeft = seg * vertsPerRing + i;
            const uint32_t topRight = seg * vertsPerRing + i + 1;
            const uint32_t bottomLeft = (seg + 1) * vertsPerRing + i;
            const uint32_t bottomRight = (seg + 1) * vertsPerRing + i + 1;

            // First triangle
            outIndices.push_back(topLeft);
            outIndices.push_back(bottomLeft);
            outIndices.push_back(topRight);

            // Second triangle
            outIndices.push_back(topRight);
            outIndices.push_back(bottomLeft);
            outIndices.push_back(bottomRight);
        }
    }
}

void ProceduralBeamGeometry::GenerateTaperedBeamGeometry(
    const glm::vec3& startPoint,
    const glm::vec3& endPoint,
    float startRadius,
    float endRadius,
    int segments,
    std::vector<BeamVertex>& outVertices,
    std::vector<uint32_t>& outIndices)
{
    outVertices.clear();
    outIndices.clear();

    if (segments < 3)
        segments = 3;

    const glm::vec3 direction = glm::normalize(endPoint - startPoint);
    glm::vec3 tangent, bitangent;
    CalculateTangentBasis(direction, tangent, bitangent);

    // Generate vertices for both caps with different radii
    for (int cap = 0; cap < 2; ++cap) {
        const glm::vec3 center = (cap == 0) ? startPoint : endPoint;
        const float radius = (cap == 0) ? startRadius : endRadius;
        const float vCoord = static_cast<float>(cap);

        for (int i = 0; i <= segments; ++i) {
            const float angle = (static_cast<float>(i) / segments) * 2.0f * glm::pi<float>();
            const float cosAngle = std::cos(angle);
            const float sinAngle = std::sin(angle);

            const glm::vec3 offset = (tangent * cosAngle + bitangent * sinAngle) * radius;
            const glm::vec3 position = center + offset;
            const glm::vec3 normal = glm::normalize(offset);
            const glm::vec2 texCoord(static_cast<float>(i) / segments, vCoord);

            outVertices.push_back({position, normal, texCoord});
        }
    }

    // Generate indices
    const int vertsPerCap = segments + 1;
    for (int i = 0; i < segments; ++i) {
        const uint32_t topLeft = i;
        const uint32_t topRight = i + 1;
        const uint32_t bottomLeft = vertsPerCap + i;
        const uint32_t bottomRight = vertsPerCap + i + 1;

        // First triangle
        outIndices.push_back(topLeft);
        outIndices.push_back(bottomLeft);
        outIndices.push_back(topRight);

        // Second triangle
        outIndices.push_back(topRight);
        outIndices.push_back(bottomLeft);
        outIndices.push_back(bottomRight);
    }
}

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

void ProceduralBeamGeometry::GenerateAnimatedElectricBeamGeometry(
    const glm::vec3& startPoint,
    const glm::vec3& endPoint,
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

    const glm::vec3 direction = glm::normalize(endPoint - startPoint);

    glm::vec3 tangent, bitangent;
    CalculateTangentBasis(direction, tangent, bitangent);

    // Generate spine points with animated jitter
    std::vector<glm::vec3> spinePoints;
    spinePoints.reserve(lengthSegments + 1);

    for (int seg = 0; seg <= lengthSegments; ++seg) {
        const float t = static_cast<float>(seg) / lengthSegments;
        glm::vec3 point = glm::mix(startPoint, endPoint, t);

        // Add animated jitter to intermediate points (not start/end)
        if (seg > 0 && seg < lengthSegments) {
            point = GetAnimatedJitteredPoint(point, jitterAmount, animationTime, animationSpeed, seg);
        }

        spinePoints.push_back(point);
    }

    // Generate vertices around each spine point
    for (int seg = 0; seg <= lengthSegments; ++seg) {
        const glm::vec3& center = spinePoints[seg];
        const float vCoord = static_cast<float>(seg) / lengthSegments;

        // Recalculate tangent basis for each segment to follow jittered spine
        glm::vec3 segmentDirection;
        if (seg == 0) {
            segmentDirection = glm::normalize(spinePoints[1] - spinePoints[0]);
        } else if (seg == lengthSegments) {
            segmentDirection = glm::normalize(spinePoints[lengthSegments] - spinePoints[lengthSegments - 1]);
        } else {
            segmentDirection = glm::normalize(spinePoints[seg + 1] - spinePoints[seg - 1]);
        }

        glm::vec3 segTangent, segBitangent;
        CalculateTangentBasis(segmentDirection, segTangent, segBitangent);

        // Add animated radius variation for pulsing effect
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

    // Generate indices
    const int vertsPerRing = radialSegments + 1;
    for (int seg = 0; seg < lengthSegments; ++seg) {
        for (int i = 0; i < radialSegments; ++i) {
            const uint32_t topLeft = seg * vertsPerRing + i;
            const uint32_t topRight = seg * vertsPerRing + i + 1;
            const uint32_t bottomLeft = (seg + 1) * vertsPerRing + i;
            const uint32_t bottomRight = (seg + 1) * vertsPerRing + i + 1;

            // First triangle
            outIndices.push_back(topLeft);
            outIndices.push_back(bottomLeft);
            outIndices.push_back(topRight);

            // Second triangle
            outIndices.push_back(topRight);
            outIndices.push_back(bottomLeft);
            outIndices.push_back(bottomRight);
        }
    }
}

void ProceduralBeamGeometry::CalculateTangentBasis(const glm::vec3& direction, glm::vec3& tangent, glm::vec3& bitangent)
{
    // Choose an arbitrary vector that's not parallel to direction
    glm::vec3 arbitrary = (std::abs(direction.y) < 0.99f) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);

    tangent = glm::normalize(glm::cross(arbitrary, direction));
    bitangent = glm::normalize(glm::cross(direction, tangent));
}

} // namespace EngineCore
