#include "ParticleExplosionEmitter.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace EngineCore {
void ParticleExplosionEmitter::EmitParticles(const size_t particlesCount)
{
    assert(mThetaSlicesCount); // set theta slices count before emitting
    assert((particlesCount % mThetaSlicesCount) == 0);
    const auto& ownerSp = mOwner.lock();
    assert(ownerSp);

    auto& particlePool = GetParticlesPool();
    const auto& poolSize = particlePool.size();

    const size_t particlesEmitCount = particlesCount <= poolSize ? particlesCount == 0 ? poolSize : particlesCount : poolSize;

    size_t particleIndex = 0;

    const size_t particlesPerPhiFullRotate = particlesEmitCount / mThetaSlicesCount;

    const float thetaStep = (EngineMath::PI * 2.0f) / (float)mThetaSlicesCount;
    const float phiStep = (EngineMath::PI) / (float)particlesPerPhiFullRotate;
    for (size_t thetaParticleIndex = 0; thetaParticleIndex < mThetaSlicesCount; ++thetaParticleIndex) {
        const float theta = thetaStep * (float)thetaParticleIndex;
        for (size_t phiParticeIndex = 0; phiParticeIndex < particlesPerPhiFullRotate; ++phiParticeIndex) {
            const float phi = phiStep * (float)phiParticeIndex;

            Particle& p = particlePool[particleIndex];
            p.Position = glm::vec3(
                mRadius * std::cos(theta) * std::sin(phi), mRadius * std::sin(theta) * std::sin(phi), mRadius * std::cos(phi));
            p.Rotation = Random::Float() * EngineMath::PI * 2;

            ++particleIndex;
        }
    }
}

void ParticleExplosionEmitter::SetExplosionRadius(const float radius)
{
    mRadius = radius;
}

void ParticleExplosionEmitter::SetThetaSlicesCount(const size_t thetaSlicesCount)
{
    mThetaSlicesCount = thetaSlicesCount;
}
} // namespace EngineCore
