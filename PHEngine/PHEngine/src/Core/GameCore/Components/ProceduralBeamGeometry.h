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

private:
    static glm::vec3 GetJitteredPoint(const glm::vec3& point, float jitterAmount);
    static glm::vec3 GetAnimatedJitteredPoint(const glm::vec3& point, float jitterAmount, float time, float speed, int seed);
    static float SmoothNoise(float x, float y, float z);
};

} // namespace EngineCore
