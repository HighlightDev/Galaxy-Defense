#include "OrbitalRingSceneProxy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/PrimitiveComponents/OrbitalRingComponent.h"

#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <vector>

using namespace EngineCore;

namespace Graphics {
namespace Proxy {
OrbitalRingSceneProxy::OrbitalRingSceneProxy(const EngineCore::OrbitalRingComponent* component)
    : RuntimeGeneratedLineSceneProxy(component)
    , mRingCenterWorldSpacePosition(component->GetRingCenterWorldSpacePosition())
    , mRingRadius(component->GetRingRadius())
    , mRingTumbleAxis(component->GetRingTumbleAxis())
    , mRingSpinAngleDeg(component->GetRingSpinAngleDeg())
    , mRingSegmentsCount(component->GetRingSegmentsCount())
{
}

void OrbitalRingSceneProxy::SetRingParams(
    const glm::vec3& center,
    const float radius,
    const glm::vec3& tumbleAxis,
    const float spinAngleDeg,
    const int32_t segmentsCount,
    const float ringWidth)
{
    mRingCenterWorldSpacePosition = center;
    mRingRadius = radius;
    mRingTumbleAxis = tumbleAxis;
    mRingSpinAngleDeg = spinAngleDeg;
    mRingSegmentsCount = segmentsCount;
    mLineWidth = ringWidth;
    bUpdateLineGeometry = true;
}

void OrbitalRingSceneProxy::UpdateGeometry(const glm::mat4& viewMatrix)
{
    if (!bUpdateLineGeometry) {
        return;
    }

    auto* const verticesVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
    auto* const textureCoordinatesVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
    ext_assert(verticesVBO && textureCoordinatesVBO, "VBOs not created for orbital ring");

    const float halfWidth = mLineWidth * 0.5f;
    const int32_t segments = mRingSegmentsCount < 3 ? 3 : mRingSegmentsCount;

    // The ring lies in the XZ plane by default; tumbling about an in-plane axis over time orbits it.
    const glm::vec3 tumbleAxis
        = glm::length(mRingTumbleAxis) > 0.0001f ? glm::normalize(mRingTumbleAxis) : glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::mat4 tumble = glm::rotate(glm::mat4(1.0f), glm::radians(mRingSpinAngleDeg), tumbleAxis);
    const glm::vec3 planeU = glm::vec3(tumble * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
    const glm::vec3 planeV = glm::vec3(tumble * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

    std::vector<float> vertices, texCoords;
    vertices.reserve(static_cast<size_t>(segments + 1) * 2 * 3);
    texCoords.reserve(static_cast<size_t>(segments + 1) * 2 * 2);

    for (int32_t i = 0; i <= segments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(segments);
        const float theta = t * glm::two_pi<float>();
        const glm::vec3 radialDir = std::cos(theta) * planeU + std::sin(theta) * planeV;
        const glm::vec3 ringPoint = mRingCenterWorldSpacePosition + radialDir * mRingRadius;
        const glm::vec3 innerEdge = ringPoint - radialDir * halfWidth;
        const glm::vec3 outerEdge = ringPoint + radialDir * halfWidth;

        vertices.insert(vertices.end(), {innerEdge.x, innerEdge.y, innerEdge.z, outerEdge.x, outerEdge.y, outerEdge.z});
        texCoords.insert(texCoords.end(), {t, 0.0f, t, 1.0f});
    }

    mVerticesCountToRender = static_cast<uint32_t>(vertices.size() / 3);

    verticesVBO->BufferSubData(0, sizeof(float) * vertices.size(), (void*)vertices.data());
    textureCoordinatesVBO->BufferSubData(0, sizeof(float) * texCoords.size(), (void*)texCoords.data());
    textureCoordinatesVBO->UnbindBuffer();

    bUpdateLineGeometry = false;
}
} // namespace Proxy
} // namespace Graphics
