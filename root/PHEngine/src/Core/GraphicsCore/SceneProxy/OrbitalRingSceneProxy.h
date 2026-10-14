#pragma once
#include "RuntimeGeneratedLineSceneProxy.h"

#include <glm/vec3.hpp>

namespace EngineCore {
class OrbitalRingComponent;
}

namespace Graphics {
namespace Proxy {
class OrbitalRingSceneProxy : public RuntimeGeneratedLineSceneProxy {
    using Base = RuntimeGeneratedLineSceneProxy;

    glm::vec3 mRingCenterWorldSpacePosition;

    float mRingRadius;

    glm::vec3 mRingTumbleAxis;

    float mRingSpinAngleDeg;

    int32_t mRingSegmentsCount;

public:
    OrbitalRingSceneProxy(const EngineCore::OrbitalRingComponent* component);

    void SetRingParams(
        const glm::vec3& center,
        const float radius,
        const glm::vec3& tumbleAxis,
        const float spinAngleDeg,
        const int32_t segmentsCount,
        const float ringWidth);

protected:
    void UpdateGeometry(const glm::mat4& viewMatrix) override;
};

} // namespace Proxy
} // namespace Graphics
