#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace EngineCore
{
    struct Particle
    {
        glm::vec3 Position{0};
        glm::vec3 InitialVelocity{0};
        glm::vec3 Velocity{0};
        glm::vec4 Color{0};
        float Size{0.0f};
        float LifeTime{0.0f};
        float LifeRemaining{0.0f};
        bool isActive = false;

        float Rotation = 0.0f;
    };
}