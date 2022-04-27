#pragma once

#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Particles/Modules/IVelocityModule.h"

namespace EngineCore
{
    class SimpleVelocityModule
        : public IVelocityModule
    {
       
       glm::vec3 mVelocityDirection;
       glm::vec3 mVelocityDeviation;

       glm::vec3 mCurrentSpawnVelocityDeviation;

    public:
       SimpleVelocityModule();

       virtual void Tick(const float deltaTime) override;

       void SetVelocityDirection(const glm::vec3& velocityDirection);

       void SetVelocityDeviation(const glm::vec3& velocityDeviation);

       virtual void OnEmitParticles() override;

    };
}