#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

#include <string>
#include <glm/vec3.hpp>

namespace EngineCore
{
   struct BillboardComponentData : public ComponentData
   {
      BillboardComponentData(const std::string &gameObjectName,
                             const float billboardExtent,
                             const glm::vec3 &translation,
                             const glm::vec3 &scale,
                             Graphics::IMaterial *material)
          : ComponentData(gameObjectName),
            m_billboardExtent(billboardExtent),
            m_translation(translation),
            m_scale(scale),
            m_material(material)
      {
      }

      float m_billboardExtent;
      glm::vec3 m_translation;
      glm::vec3 m_scale;

      Graphics::IMaterial *m_material;
   };

}