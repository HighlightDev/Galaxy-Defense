#include "GravityGridSceneProxy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/PrimitiveComponents/GravityGridComponent.h"

#include <glm/common.hpp>
#include <glm/geometric.hpp>

#include <vector>

using namespace EngineCore;

namespace Graphics {
namespace Proxy {
GravityGridSceneProxy::GravityGridSceneProxy(const EngineCore::GravityGridComponent* component)
    : RuntimeGeneratedLineSceneProxy(component)
    , mGridCenterWorldSpacePosition(component->GetGridCenterWorldSpacePosition())
    , mGridHalfExtent(component->GetGridHalfExtent())
    , mGridLineCount(component->GetGridLineCount())
    , mSegmentsPerLine(component->GetSegmentsPerLine())
    , mDeformStrength(component->GetDeformStrength())
    , mDipStrength(component->GetDipStrength())
    , mSoftening(component->GetSoftening())
{
}

void GravityGridSceneProxy::SetGridParams(
    const glm::vec3& center,
    const float halfExtent,
    const int32_t lineCount,
    const int32_t segmentsPerLine,
    const float deformStrength,
    const float dipStrength,
    const float softening,
    const float gridLineWidth)
{
    mGridCenterWorldSpacePosition = center;
    mGridHalfExtent = halfExtent;
    mGridLineCount = lineCount;
    mSegmentsPerLine = segmentsPerLine;
    mDeformStrength = deformStrength;
    mDipStrength = dipStrength;
    mSoftening = softening;
    mLineWidth = gridLineWidth;
    bUpdateLineGeometry = true;
}

void GravityGridSceneProxy::UpdateGeometry(const glm::mat4& viewMatrix)
{
    if (!bUpdateLineGeometry) {
        return;
    }

    auto* const verticesVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
    auto* const textureCoordinatesVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
    ext_assert(verticesVBO && textureCoordinatesVBO, "VBOs not created for gravity grid");

    const float halfWidth = mLineWidth * 0.5f;
    const int32_t lineCount = mGridLineCount < 2 ? 2 : mGridLineCount;
    const int32_t segments = mSegmentsPerLine < 1 ? 1 : mSegmentsPerLine;

    const glm::vec3 center = mGridCenterWorldSpacePosition;
    const float minX = center.x - mGridHalfExtent;
    const float maxX = center.x + mGridHalfExtent;
    const float minZ = center.z - mGridHalfExtent;
    const float maxZ = center.z + mGridHalfExtent;

    // Pulls a flat grid point toward the center in the XZ plane and dips it in Y, both falling off as ~1/dist.
    const auto deform = [&](const glm::vec3& flatPoint) {
        const glm::vec2 toCenter = glm::vec2(center.x, center.z) - glm::vec2(flatPoint.x, flatPoint.z);
        const float dist = glm::length(toCenter);
        glm::vec3 result = flatPoint;
        if (dist > 0.0001f) {
            const glm::vec2 dir = toCenter / dist;
            const float pull = glm::min(mDeformStrength / (dist + mSoftening), dist * 0.9f);
            result.x += dir.x * pull;
            result.z += dir.y * pull;
        }
        result.y -= mDipStrength / (dist + mSoftening);
        return result;
    };

    std::vector<float> vertices;
    std::vector<float> texCoords;
    const size_t approxVertexCount = static_cast<size_t>(lineCount) * 2 * (static_cast<size_t>(segments) + 1) * 2 + 64;
    vertices.reserve(approxVertexCount * 3);
    texCoords.reserve(approxVertexCount * 2);

    bool isFirstRibbon = true;
    glm::vec3 lastVertexPos(0.0f);
    glm::vec2 lastVertexUv(0.0f);

    const auto appendVertex = [&](const glm::vec3& p, const glm::vec2& uv) {
        vertices.insert(vertices.end(), {p.x, p.y, p.z});
        texCoords.insert(texCoords.end(), {uv.x, uv.y});
        lastVertexPos = p;
        lastVertexUv = uv;
    };

    // Emits one deformed grid line as a ribbon, stitched to the previous ribbon with degenerate triangles. Backface
    // culling is disabled for runtime lines, so the winding across the degenerate joins does not matter.
    const auto emitLine = [&](const glm::vec3& lineBegin, const glm::vec3& lineEnd, const glm::vec3& widthAxis) {
        for (int32_t si = 0; si <= segments; ++si) {
            const float t = static_cast<float>(si) / static_cast<float>(segments);
            const glm::vec3 deformedPoint = deform(glm::mix(lineBegin, lineEnd, t));
            const glm::vec3 left = deformedPoint - widthAxis * halfWidth;
            const glm::vec3 right = deformedPoint + widthAxis * halfWidth;

            if (si == 0 && !isFirstRibbon) {
                appendVertex(lastVertexPos, lastVertexUv); // close previous ribbon
                appendVertex(left, glm::vec2(t, 0.0f));    // start of this ribbon
            }

            appendVertex(left, glm::vec2(t, 0.0f));
            appendVertex(right, glm::vec2(t, 1.0f));
        }
        isFirstRibbon = false;
    };

    const glm::vec3 axisX(1.0f, 0.0f, 0.0f);
    const glm::vec3 axisZ(0.0f, 0.0f, 1.0f);

    // Lines running along X (constant Z), width measured across Z.
    for (int32_t li = 0; li < lineCount; ++li) {
        const float z = glm::mix(minZ, maxZ, static_cast<float>(li) / static_cast<float>(lineCount - 1));
        emitLine(glm::vec3(minX, center.y, z), glm::vec3(maxX, center.y, z), axisZ);
    }
    // Lines running along Z (constant X), width measured across X.
    for (int32_t li = 0; li < lineCount; ++li) {
        const float x = glm::mix(minX, maxX, static_cast<float>(li) / static_cast<float>(lineCount - 1));
        emitLine(glm::vec3(x, center.y, minZ), glm::vec3(x, center.y, maxZ), axisX);
    }

    mVerticesCountToRender = static_cast<uint32_t>(vertices.size() / 3);

    verticesVBO->BufferSubData(0, sizeof(float) * vertices.size(), (void*)vertices.data());
    textureCoordinatesVBO->BufferSubData(0, sizeof(float) * texCoords.size(), (void*)texCoords.data());
    textureCoordinatesVBO->UnbindBuffer();

    bUpdateLineGeometry = false;
}
} // namespace Proxy
} // namespace Graphics
