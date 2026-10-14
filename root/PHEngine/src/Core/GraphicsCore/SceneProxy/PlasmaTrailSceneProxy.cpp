#include "PlasmaTrailSceneProxy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/PrimitiveComponents/PlasmaTrailComponent.h"

#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>

#include <vector>

using namespace EngineCore;

namespace Graphics {
namespace Proxy {
PlasmaTrailSceneProxy::PlasmaTrailSceneProxy(const EngineCore::PlasmaTrailComponent* component)
    : RuntimeGeneratedLineSceneProxy(component)
    , mTrailPoints(component->GetTrailPointsSnapshot())
{
}

void PlasmaTrailSceneProxy::SetTrailParams(const std::vector<glm::vec3>& trailPoints, const float width)
{
    mTrailPoints = trailPoints;
    mLineWidth = width;
    bUpdateLineGeometry = true;
}

void PlasmaTrailSceneProxy::UpdateGeometry(const glm::mat4& viewMatrix)
{
    if (!bUpdateLineGeometry) {
        return;
    }

    auto* const verticesVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
    auto* const textureCoordinatesVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
    ext_assert(verticesVBO && textureCoordinatesVBO, "VBOs not created for plasma trail");

    std::vector<float> vertices;
    std::vector<float> texCoords;

    const int32_t pointCount = static_cast<int32_t>(mTrailPoints.size());
    if (pointCount < 2) {
        // Not enough samples to form a ribbon: emit nothing this frame.
        mVerticesCountToRender = 0;
        verticesVBO->BufferSubData(0, 0, (void*)vertices.data());
        textureCoordinatesVBO->BufferSubData(0, 0, (void*)texCoords.data());
        textureCoordinatesVBO->UnbindBuffer();
        bUpdateLineGeometry = false;
        return;
    }

    const float halfWidth = mLineWidth * 0.5f;
    const glm::mat4 camToWorld = glm::inverse(viewMatrix);
    const glm::vec3 camForward = -glm::normalize(glm::vec3(camToWorld[2]));

    vertices.reserve(static_cast<size_t>(pointCount) * 2 * 3);
    texCoords.reserve(static_cast<size_t>(pointCount) * 2 * 2);

    // Index 0 is the oldest sample (tail), pointCount-1 is the newest (head). Width tapers from ~0 at the tail to full
    // at the head; texcoord.x runs 0 (tail) -> 1 (head) so the material gradient/flow runs along the trail.
    for (int32_t i = 0; i < pointCount; ++i) {
        const glm::vec3& prev = mTrailPoints[i == 0 ? 0 : i - 1];
        const glm::vec3& next = mTrailPoints[i == pointCount - 1 ? pointCount - 1 : i + 1];
        glm::vec3 tangent = next - prev;
        tangent = glm::length(tangent) > 0.0001f ? glm::normalize(tangent) : camForward;

        glm::vec3 widthAxis = glm::cross(tangent, camForward);
        widthAxis = glm::length(widthAxis) > 0.0001f ? glm::normalize(widthAxis) : glm::vec3(0.0f, 1.0f, 0.0f);

        const float t = static_cast<float>(i) / static_cast<float>(pointCount - 1);
        const float taperedHalfWidth = halfWidth * t; // wide at head, vanishing at tail

        const glm::vec3 left = mTrailPoints[i] - widthAxis * taperedHalfWidth;
        const glm::vec3 right = mTrailPoints[i] + widthAxis * taperedHalfWidth;

        vertices.insert(vertices.end(), {left.x, left.y, left.z, right.x, right.y, right.z});
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
