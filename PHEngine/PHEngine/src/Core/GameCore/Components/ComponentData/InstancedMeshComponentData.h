#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

#include <string>
#include <glm/vec3.hpp>

namespace EngineCore
{
   struct InstancedMeshComponentData : public ComponentData
   {
      InstancedMeshComponentData(const std::string &gameObjectName,
                                 const std::string &pathToMesh,
                                 const glm::vec3 &translation,
                                 const glm::vec3 &eulerRotationDegrees,
                                 const glm::vec3 &scale,
                                 const std::shared_ptr<Graphics::IMaterial> &material)
          : ComponentData(gameObjectName),
            m_pathToMesh(pathToMesh),
            m_translation(translation),
            m_eulerRotationDegrees(eulerRotationDegrees),
            m_scale(scale),
            m_material(material)
      {
      }

      std::string m_pathToMesh;
      glm::vec3 m_translation;
      glm::vec3 m_eulerRotationDegrees;
      glm::vec3 m_scale;

      std::shared_ptr<Graphics::IMaterial> m_material;

      virtual ~InstancedMeshComponentData() {}
   };
}