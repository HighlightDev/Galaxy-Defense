#pragma once
#include "RuntimeGeneratedLineSceneProxy.h"

#include <glm/vec3.hpp>

namespace EngineCore {
class GravityGridComponent;
}

namespace Graphics {
namespace Proxy {
class GravityGridSceneProxy : public RuntimeGeneratedLineSceneProxy {
    using Base = RuntimeGeneratedLineSceneProxy;

    glm::vec3 mGridCenterWorldSpacePosition;

    float mGridHalfExtent;

    int32_t mGridLineCount;

    int32_t mSegmentsPerLine;

    float mDeformStrength;

    float mDipStrength;

    float mSoftening;

public:
    GravityGridSceneProxy(const EngineCore::GravityGridComponent* component);

    void SetGridParams(
        const glm::vec3& center,
        const float halfExtent,
        const int32_t lineCount,
        const int32_t segmentsPerLine,
        const float deformStrength,
        const float dipStrength,
        const float softening,
        const float gridLineWidth);

protected:
    void UpdateGeometry(const glm::mat4& viewMatrix) override;
};

} // namespace Proxy
} // namespace Graphics
