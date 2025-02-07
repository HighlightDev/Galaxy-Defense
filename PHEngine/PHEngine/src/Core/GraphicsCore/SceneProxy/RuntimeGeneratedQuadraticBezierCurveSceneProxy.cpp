#include "RuntimeGeneratedQuadraticBezierCurveSceneProxy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace EngineCore;
using namespace Graphics::Renderer;
using namespace Graphics;

namespace Graphics {
namespace Proxy {
RuntimeGeneratedQuadraticBezierCurveSceneProxy::RuntimeGeneratedQuadraticBezierCurveSceneProxy(
    const RuntimeGeneratedQuadraticBezierCurveComponent* component)
    : RuntimeGeneratedLineSceneProxy(component)
    , mBezierControlPointWorldSpacePosition(component->GetBezierControlPointWorldSpacePosition())
    , mCurveSegmentsCount(component->GetCurveSegmentsCount())
{
}

void RuntimeGeneratedQuadraticBezierCurveSceneProxy::SetBezierControlPointWorldSpacePosition(const glm::vec3& position)
{
    mBezierControlPointWorldSpacePosition = position;
    bUpdateLineGeometry = true;
}

void RuntimeGeneratedQuadraticBezierCurveSceneProxy::SetCurveSegmentsCount(const int32_t curveSegmentsCount)
{
    mCurveSegmentsCount = curveSegmentsCount;
    bUpdateLineGeometry = true;
}

void RuntimeGeneratedQuadraticBezierCurveSceneProxy::UpdateGeometry(const glm::mat4& viewMatrix)
{
    if (bUpdateLineGeometry) {
        auto* const verticesVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
        auto* const textureCoordinatesVBO = m_skin->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");

        assert(verticesVBO && textureCoordinatesVBO);
        const float halfWidth = mLineWidth * 0.5f;

        // Generate curve path with quadratic Bezier function
        const size_t curvePathPointsCount = mCurveSegmentsCount <= static_cast<size_t>(1)
            ? static_cast<size_t>(2)
            : static_cast<size_t>(mCurveSegmentsCount + 1);
        std::vector<glm::vec3> curvePathPoints;
        curvePathPoints.reserve(curvePathPointsCount);
        curvePathPoints.emplace_back(mLineBeginWorldSpacePosition);
        const float bezier_t_step = (1.0f / static_cast<float>(curvePathPointsCount));
        float bezier_t = 0.0f;
        for (size_t i = 1; i < (curvePathPointsCount); ++i) {
            bezier_t = static_cast<float>(i) * bezier_t_step;
            curvePathPoints.emplace_back(EngineMath::QuadraticBezier(
                mLineBeginWorldSpacePosition, mBezierControlPointWorldSpacePosition, mLineEndWorldSpacePosition, bezier_t));
        }
        curvePathPoints.emplace_back(mLineEndWorldSpacePosition);

        // Fill array of vertices and texture coordinates
        std::vector<float> vertices, texCoords;
        vertices.reserve(curvePathPointsCount * static_cast<size_t>(2 * 3));
        texCoords.reserve(curvePathPointsCount * static_cast<size_t>(2 * 2));
        const float texCoordOffsetX = 1.0f / static_cast<float>(curvePathPointsCount - 1);
        const auto pathPointsCount = curvePathPoints.size();
        for (int i = 1; i < pathPointsCount; ++i) {
            const auto& prevPosition = curvePathPoints[i - 1];
            const auto& currentPosition = curvePathPoints[i];

            const auto& worldForwardVec = glm::normalize(currentPosition - prevPosition);
            const auto& worldCameraUpVec = glm::vec3(0.0f, 1.0f, 0.0f);
            const auto& lineBinormalVector = glm::normalize(glm::cross(worldCameraUpVec, worldForwardVec));

            const auto viewP3 = currentPosition - (lineBinormalVector * halfWidth);
            const auto viewP4 = currentPosition + (lineBinormalVector * halfWidth);

            const float x_texCoord = texCoordOffsetX * static_cast<float>(i);
            const auto texP3 = glm::vec2(x_texCoord, 1.0f);
            const auto texP4 = glm::vec2(x_texCoord, 0.0f);

            const auto currentVerticesOffset = vertices.size();
            using difference_type = typename std::iterator_traits<decltype(vertices.begin())>::difference_type;

            if (1 == i) {
                const auto viewP1 = prevPosition - (lineBinormalVector * halfWidth);
                const auto viewP2 = prevPosition + (lineBinormalVector * halfWidth);
                const auto texP1 = glm::vec2(0.0f, 1.0f);
                const auto texP2 = glm::vec2(0.0f, 0.0f);
                vertices.insert(
                    std::next(vertices.begin(), static_cast<difference_type>(currentVerticesOffset)),
                    {viewP1.x,
                     viewP1.y,
                     viewP1.z,
                     viewP2.x,
                     viewP2.y,
                     viewP2.z,
                     viewP3.x,
                     viewP3.y,
                     viewP3.z,
                     viewP4.x,
                     viewP4.y,
                     viewP4.z});

                const auto currentTexCoordsOffset = texCoords.size();
                texCoords.insert(
                    std::next(texCoords.begin(), static_cast<difference_type>(currentTexCoordsOffset)),
                    {texP1.x, texP1.y, texP2.x, texP2.y, texP3.x, texP3.y, texP4.x, texP4.y});
            } else {
                vertices.insert(
                    std::next(vertices.begin(), static_cast<difference_type>(currentVerticesOffset)),
                    {viewP3.x, viewP3.y, viewP3.z, viewP4.x, viewP4.y, viewP4.z});

                const auto currentTexCoordsOffset = texCoords.size();
                texCoords.insert(
                    std::next(texCoords.begin(), static_cast<difference_type>(currentTexCoordsOffset)),
                    {texP3.x, texP3.y, texP4.x, texP4.y});
            }
        }

        mVerticesCountToRender = vertices.size() / static_cast<uint32_t>(3);

        const size_t verticesBufferSize = sizeof(float) * vertices.size();
        const size_t texCoordsBufferSize = sizeof(float) * texCoords.size();

        verticesVBO->BindVBO();
        verticesVBO->BufferSubData(0, verticesBufferSize, (void*)vertices.data());

        textureCoordinatesVBO->BindVBO();
        textureCoordinatesVBO->BufferSubData(0, texCoordsBufferSize, (void*)texCoords.data());
        textureCoordinatesVBO->UnbindVBO();

        bUpdateLineGeometry = false;
    }
}
} // namespace Proxy
} // namespace Graphics
