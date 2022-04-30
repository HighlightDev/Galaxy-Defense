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
        glm::vec4 Color;
        float Size;
        float LifeTime;
        float LifeRemaining;
        bool isActive = false;

        float Rotation = 0.0f;
    };
}