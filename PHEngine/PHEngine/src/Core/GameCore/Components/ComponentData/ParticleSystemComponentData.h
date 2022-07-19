#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

#include <string>
#include <glm/vec3.hpp>

namespace EngineCore
{
   struct ParticleSystemComponentData : public ComponentData
   {
      ParticleSystemComponentData(const std::string &gameObjectName,
                                  Graphics::IMaterial *materialInstance,
                                  const glm::vec3 &translation,
                                  const size_t particlesCount)
          : ComponentData(gameObjectName),
            m_translation(translation),
            m_particlesCount(particlesCount),
            m_material(materialInstance)
      {
      }

      glm::vec3 m_translation;
      size_t m_particlesCount;
      Graphics::IMaterial *m_material;
   };

}