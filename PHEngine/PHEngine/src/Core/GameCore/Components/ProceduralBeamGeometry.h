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

    // Generate cylindrical beam geometry
    static void GenerateBeamGeometry(
        const glm::vec3& startPoint,
        const glm::vec3& endPoint,
        float radius,
        int segments,
        std::vector<BeamVertex>& outVertices,
        std::vector<uint32_t>& outIndices);

    // Generate beam with jittered segments for electric effect
    static void GenerateElectricBeamGeometry(
        const glm::vec3& startPoint,
        const glm::vec3& endPoint,
        float radius,
        int radialSegments,
        int lengthSegments,
        float jitterAmount,
        std::vector<BeamVertex>& outVertices,
        std::vector<uint32_t>& outIndices);

    // Generate beam with animated jitter using time-based noise
    static void GenerateAnimatedElectricBeamGeometry(
        const glm::vec3& startPoint,
        const glm::vec3& endPoint,
        float radius,
        int radialSegments,
        int lengthSegments,
        float jitterAmount,
        float animationTime,
        float animationSpeed,
        std::vector<BeamVertex>& outVertices,
        std::vector<uint32_t>& outIndices);

    // Generate tapered beam (cone-like)
    static void GenerateTaperedBeamGeometry(
        const glm::vec3& startPoint,
        const glm::vec3& endPoint,
        float startRadius,
        float endRadius,
        int segments,
        std::vector<BeamVertex>& outVertices,
        std::vector<uint32_t>& outIndices);

private:
    static glm::vec3 GetJitteredPoint(const glm::vec3& point, float jitterAmount);
    static glm::vec3 GetAnimatedJitteredPoint(const glm::vec3& point, float jitterAmount, float time, float speed, int seed);
    static void CalculateTangentBasis(const glm::vec3& direction, glm::vec3& tangent, glm::vec3& bitangent);
    static float SmoothNoise(float x, float y, float z);
};

} // namespace EngineCore
