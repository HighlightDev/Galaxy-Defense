#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"

#include <string>
#include <glm/vec3.hpp>

namespace EngineCore
{
   struct ParticleSystemComponentData : public ComponentData
   {
      ParticleSystemComponentData(const std::string &gameObjectName,
                                  const glm::vec3 &translation,
                                  const size_t particlesCount)
          : ComponentData(gameObjectName),
            m_translation(translation),
            m_particlesCount(particlesCount)
      {
      }

      glm::vec3 m_translation;
      size_t m_particlesCount;
   };

}