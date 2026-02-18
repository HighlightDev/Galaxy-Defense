#include "SimpleColorModule.h"

#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace EngineCore {
SimpleColorModule::SimpleColorModule()
    : mColorBegin()
    , mColorEnd()
{
}

void SimpleColorModule::Update(Particle& particle, const float deltaTimeSec)
{
    particle.Color = LerpVec3(particle.LifeTime - particle.LifeRemaining, 0.0f, particle.LifeTime, mColorBegin, mColorEnd);
}

void SimpleColorModule::SetColorBegin(const glm::vec3& colorBegin)
{
    mColorBegin = colorBegin;
}

void SimpleColorModule::SetColorEnd(const glm::vec3& colorEnd)
{
    mColorEnd = colorEnd;
}

void SimpleColorModule::OnEmitParticles()
{
}
} // namespace EngineCore