#include "ScreenRayCaster.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

#include <glm/matrix.hpp>

namespace EngineCore
{
    glm::vec3 ScreenRayCaster::CastRayFromScreenSpaceToWorldSpace(const glm::ivec2 &screenSpacePosition,
                                                                  const glm::ivec2 &windowSize,
                                                                  const glm::mat4 &projectionMatrix,
                                                                  const glm::mat4 &viewMatrix) const
    {
        const auto &nSSPosition = GetNormalizedScreenSpacePosition(screenSpacePosition, windowSize);
        const auto &ndcPosition = (nSSPosition * 2.0f) - 1.0f; // convert from [0; 1] to [-1; 1]
        const auto &clippedSpacePosition = glm::vec4(ndcPosition.x, -ndcPosition.y, -1.0f, 1.0f);
        const auto &viewSpaceRay = GetViewSpaceRay(clippedSpacePosition, projectionMatrix);
        const auto &worldSpaceRay = GetWorldSpaceRay(viewSpaceRay, viewMatrix);
        return worldSpaceRay;
    }

    glm::vec2 ScreenRayCaster::GetNormalizedScreenSpacePosition(const glm::ivec2 &screenSpacePosition,
                                                                const glm::ivec2 &windowSize) const
    {
        return glm::vec2(static_cast<float>(screenSpacePosition.x) / static_cast<float>(windowSize.x),
                         static_cast<float>(screenSpacePosition.y) / static_cast<float>(windowSize.y));
    }

    glm::vec4 ScreenRayCaster::GetViewSpaceRay(const glm::vec4 &clippedSpacePosition, const glm::mat4 &projectionMatrix) const
    {
        assert(!EngineMath::FloatsNearEqual(glm::determinant(projectionMatrix), 0.0f));
        const auto &invProjectionMatrix = glm::inverse(projectionMatrix);
        const auto viewSpacePosition = invProjectionMatrix * clippedSpacePosition;
        return glm::vec4(viewSpacePosition.x, viewSpacePosition.y, -1.0f, 0.0f);
    }

    glm::vec3 ScreenRayCaster::GetWorldSpaceRay(const glm::vec4 &viewSpaceVec, const glm::mat4 &viewMatrix) const
    {
        assert(!EngineMath::FloatsNearEqual(glm::determinant(viewMatrix), 0.0f));
        const auto &invViewMatrix = glm::inverse(viewMatrix);
        const auto &worldSpaceVec = invViewMatrix * viewSpaceVec;
        const auto &nWorldSpaceRay = glm::normalize(glm::vec3(worldSpaceVec.x, worldSpaceVec.y, worldSpaceVec.z));
        return nWorldSpaceRay;
    }

}
