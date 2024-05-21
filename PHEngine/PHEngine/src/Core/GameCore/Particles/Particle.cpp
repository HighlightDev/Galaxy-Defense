#include "Particle.h"

namespace EngineCore
{
    void Particle::Reset()
    {
        isActive = false;
        Position = {};
        InitialVelocity = {};
        Velocity = {};
        Color = {};
        Size = {0.0f};
        LifeTime = {0.0f};
        LifeRemaining = {0.0f};
        Rotation = 0.0f;
    }
}
