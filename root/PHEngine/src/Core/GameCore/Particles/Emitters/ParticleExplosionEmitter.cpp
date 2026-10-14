#include "ParticleExplosionEmitter.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemBaseComponent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace EngineCore {
void ParticleExplosionEmitter::EmitParticles(const size_t particlesCount)
{
    ext_assert(
        mThetaSlicesCount,
        "ParticleExplosionEmitter::EmitParticles: mThetaSlicesCount is zero"); // set theta slices count before emitting
    ext_assert(
        (particlesCount % mThetaSlicesCount) == 0,
        "ParticleExplosionEmitter::EmitParticles: particlesCount must be multiple of mThetaSlicesCount");
    const auto& ownerSp = mOwner.lock();
    ext_assert(ownerSp, "ParticleExplosionEmitter::EmitParticles: owner is null");

    LogInfo(
        "ParticleExplosionEmitter::EmitParticles: particles count: ",
        particlesCount,
        ", radius: ",
        mRadius,
        ", theta slices count: ",
        mThetaSlicesCount,
        ", owner ID: ",
        ownerSp->GetObjectId(),
        ", owner name: ",
        ownerSp->GetEngineObjectName());

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
            p.Reset();
            p.Position = glm::vec3(
                mRadius * std::cos(theta) * std::sin(phi), mRadius * std::sin(theta) * std::sin(phi), mRadius * std::cos(phi));
            p.Rotation = Random::Float() * EngineMath::PI * 2;

            ++particleIndex;
        }
    }
}

void ParticleExplosionEmitter::EmitSingleParticle(Particle& particle)
{
    const float theta = Random::Float() * EngineMath::PI * 2.0f;
    const float phi = Random::Float() * EngineMath::PI;

    particle.Reset();
    particle.Position = glm::vec3(
        mRadius * std::cos(theta) * std::sin(phi), mRadius * std::sin(theta) * std::sin(phi), mRadius * std::cos(phi));
    particle.Rotation = Random::Float() * EngineMath::PI * 2;
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
