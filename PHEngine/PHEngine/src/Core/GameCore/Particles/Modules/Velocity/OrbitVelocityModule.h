#pragma once

#include "Core/GameCore/Particles/Modules/Velocity/IVelocityModule.h"

namespace EngineCore {
enum class eOrbitExtraVelocityDirectionType { None, Inside, Outside };

class OrbitVelocityModule : public IVelocityModule {

    eOrbitExtraVelocityDirectionType mExtraVelocityDirectionType{eOrbitExtraVelocityDirectionType::None};

    float mExtraVelocityPower{0.0f};

public:
    OrbitVelocityModule();

    void Update(Particle& particle, const float deltaTime) override;

    void OnEmitParticles() override;

    void SetExtraVelocityDirectionType(const eOrbitExtraVelocityDirectionType velocityType);

    void SetExtraVelocityPower(const float velocityPower);
};
} // namespace EngineCore