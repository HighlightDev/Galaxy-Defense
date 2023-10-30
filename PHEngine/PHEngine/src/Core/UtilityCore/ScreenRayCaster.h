#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace EngineCore
{
    class ScreenRayCaster
    {
    public:
        glm::vec3 CastRayFromScreenSpaceToWorldSpace(const glm::ivec2 &screenSpacePosition,
                                                     const glm::ivec2 &windowSize,
                                                     const glm::mat4 &projectionMatrix,
                                                     const glm::mat4 &viewMatrix) const;

    private:
        glm::vec2 GetNormalizedScreenSpacePosition(const glm::ivec2 &screenSpacePosition,
                                                   const glm::ivec2 &windowSize) const;

        glm::vec4 GetViewSpaceRay(const glm::vec4 &clippedSpacePosition, const glm::mat4 &projectionMatrix) const;

        glm::vec3 GetWorldSpaceRay(const glm::vec4 &viewSpaceVec, const glm::mat4 &viewMatrix) const;
    };
}
