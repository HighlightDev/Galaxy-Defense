#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"

#include <string>
#include <glm/vec3.hpp>

namespace EngineCore
{
   struct BillboardComponentData : public ComponentData
   {
      BillboardComponentData(const std::string &gameObjectName,
                             const float billboardExtent,
                             const std::string &pathToTexture,
                             const glm::vec3 &translation,
                             const glm::vec3 &scale)
          : ComponentData(gameObjectName),
            m_billboardExtent(billboardExtent),
            m_pathToTexture(pathToTexture),
            m_translation(translation),
            m_scale(scale)

      {
      }

      float m_billboardExtent;
      glm::vec3 m_translation;
      glm::vec3 m_scale;
      std::string m_pathToTexture;
   };

}