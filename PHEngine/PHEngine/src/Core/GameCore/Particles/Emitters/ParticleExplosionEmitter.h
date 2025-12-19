#pragma once

#include "IEmitter.h"

namespace EngineCore {
class ParticleExplosionEmitter : public IEmitter {

    float mRadius = 1.0f;

    size_t mThetaSlicesCount{0};

    void EmitParticles(const size_t particlesCount = 0) override;

public:
    void SetExplosionRadius(const float radius);

    void SetThetaSlicesCount(const size_t thetaSlicesCount);
};

} // namespace EngineCore
