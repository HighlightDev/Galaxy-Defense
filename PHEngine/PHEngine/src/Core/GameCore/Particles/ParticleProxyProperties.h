#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace EngineCore
{
    struct ParticleProxyProperties
    {
        glm::vec3 Position;
        glm::vec4 Color;
        float Rotation = 0.0f;
        float Size;
    };
}