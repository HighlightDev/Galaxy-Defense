#include "OrbitVelocityModule.h"

#include "Core/CommonCore/Random.h"
#include "Core/GameCore/Components/ParticleComponents/ParticleSystemComponent.h"
#include "Core/UtilityCore/EngineMath.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace EngineMath;

namespace EngineCore
{
   OrbitVelocityModule::OrbitVelocityModule()
   {
   }

   void OrbitVelocityModule::Update(Particle &particle, const float deltaTime)
   {
      if (auto ownerSp = mOwner.lock())
      {
         const auto orbitOrigin = ownerSp->GetHierarchyAccumulatedTranslation();
         const auto& particlePosition = particle.Position;
         const auto nOrbitDir = glm::normalize(particlePosition - orbitOrigin);
         static constexpr float rotationStepDeg = 10.0f;

         glm::mat4 identityMatrix(1);
         const glm::mat4 yawRotation = glm::rotate(identityMatrix, DEG_TO_RAD(rotationStepDeg), AXIS_UP);
         glm::vec4 rotatedOrbitDir = yawRotation * glm::vec4(nOrbitDir, 0.0);
         glm::vec3 tangentVec = glm::cross(glm::vec3(rotatedOrbitDir), AXIS_UP);
         particle.Velocity = tangentVec;
         
         if (eOrbitExtraVelocityDirectionType::Inside == mExtraVelocityDirectionType)
         {
            particle.Velocity = glm::normalize(particle.Velocity + (-nOrbitDir * mExtraVelocityPower));
         }
         else if (eOrbitExtraVelocityDirectionType::Outside == mExtraVelocityDirectionType)
         {
            particle.Velocity = glm::normalize(particle.Velocity + (nOrbitDir * mExtraVelocityPower));
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
}