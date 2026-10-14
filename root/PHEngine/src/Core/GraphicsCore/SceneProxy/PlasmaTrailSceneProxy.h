#pragma once
#include "RuntimeGeneratedLineSceneProxy.h"

#include <glm/vec3.hpp>

#include <vector>

namespace EngineCore {
class PlasmaTrailComponent;
}

namespace Graphics {
namespace Proxy {
class PlasmaTrailSceneProxy : public RuntimeGeneratedLineSceneProxy {
    using Base = RuntimeGeneratedLineSceneProxy;

    std::vector<glm::vec3> mTrailPoints;

public:
    PlasmaTrailSceneProxy(const EngineCore::PlasmaTrailComponent* component);

    void SetTrailParams(const std::vector<glm::vec3>& trailPoints, const float width);

protected:
    void UpdateGeometry(const glm::mat4& viewMatrix) override;
};

} // namespace Proxy
} // namespace Graphics
