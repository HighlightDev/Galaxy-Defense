#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace EngineCore {
struct ParticleProperties {
    glm::vec3 Position;
    glm::vec3 Velocity, VelocityVariation;
    glm::vec4 ColorBegin, ColorEnd;
    float SizeBegin, SizeEnd, SizeVariation;
    float LifeTime = 1.0f;
};
} // namespace EngineCore