#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedQuadraticBezierCurveComponent.h"
#include "RuntimeGeneratedLineSceneProxy.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

using namespace EngineCore;

namespace Graphics {
namespace Proxy {
class RuntimeGeneratedQuadraticBezierCurveSceneProxy : public RuntimeGeneratedLineSceneProxy {
    using Base = RuntimeGeneratedLineSceneProxy;

    glm::vec3 mBezierControlPointWorldSpacePosition;

    int32_t mCurveSegmentsCount;

public:
    RuntimeGeneratedQuadraticBezierCurveSceneProxy(const RuntimeGeneratedQuadraticBezierCurveComponent* component);

    void SetBezierControlPointWorldSpacePosition(const glm::vec3& position);

    void SetCurveSegmentsCount(const int32_t curveSegmentsCount);

protected:
    void UpdateGeometry(const glm::mat4& viewMatrix) override;
};

} // namespace Proxy
} // namespace Graphics
