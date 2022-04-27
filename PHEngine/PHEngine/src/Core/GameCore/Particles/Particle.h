#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace EngineCore
{
    struct Particle
    {
        glm::vec3 Position;
        glm::vec3 InitialVelocity;
        glm::vec3 Velocity;
        glm::vec4 ColorBegin, ColorEnd;
        float Rotation = 0.0f;
        float SizeBegin, SizeEnd;

        float LifeTime = 1.0f;
        float LifeRemaining = 0.0f;

        bool isActive = false;
    };
}