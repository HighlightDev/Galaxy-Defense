#include "OrbitVelocityModule.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemBaseComponent.h"
#include "Core/GameCore/Particles/Modules/ModuleGpuProxy/Velocity/OrbitVelocityModuleGpuProxy.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

using namespace EngineMath;

namespace EngineCore {
OrbitVelocityModule::OrbitVelocityModule()
{
}

void OrbitVelocityModule::Update(Particle& particle, const float deltaTimeSec)
{
    if (auto ownerSp = mOwner.lock()) {
        const auto orbitOrigin = ownerSp->GetHierarchyAccumulatedTranslation();
        const auto& particlePosition = particle.Position;
        const auto nOrbitDir = glm::normalize(particlePosition - orbitOrigin);
        const float rotationStepDeg = deltaTimeSec * 100.0f;

        glm::mat4 identityMatrix(1);
        const glm::mat4 yawRotation = glm::rotate(identityMatrix, DEG_TO_RAD(rotationStepDeg), AXIS_UP);
        glm::vec4 rotatedOrbitDir = yawRotation * glm::vec4(nOrbitDir, 0.0);
        glm::vec3 tangentVec = glm::cross(glm::vec3(rotatedOrbitDir), AXIS_UP);
        particle.Velocity = tangentVec;

        if (eOrbitExtraVelocityDirectionType::Inside == mExtraVelocityDirectionType) {
            particle.Velocity = deltaTimeSec * glm::normalize(particle.Velocity + (-nOrbitDir * mExtraVelocityPower));
        } else if (eOrbitExtraVelocityDirectionType::Outside == mExtraVelocityDirectionType) {
            particle.Velocity = deltaTimeSec * glm::normalize(particle.Velocity + (nOrbitDir * mExtraVelocityPower));
        }
    }
}

void OrbitVelocityModule::OnEmitParticles()
{
}

void OrbitVelocityModule::SetExtraVelocityDirectionType(const eOrbitExtraVelocityDirectionType velocityType)
{
    mExtraVelocityDirectionType = velocityType;
}

void OrbitVelocityModule::SetExtraVelocityPower(const float velocityPower)
{
    mExtraVelocityPower = velocityPower;
}

std::shared_ptr<IGpuParticleModuleProxy> OrbitVelocityModule::GetGpuProxy() const
{
    return std::make_shared<OrbitVelocityModuleGpuProxy>(mExtraVelocityDirectionType, mExtraVelocityPower);
}
} // namespace EngineCore