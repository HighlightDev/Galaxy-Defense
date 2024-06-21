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
                        const std::shared_ptr<Graphics::IMaterial> &material)
          : ComponentData(gameObjectName),
            m_pathToMesh(pathToMesh),
            m_material(material)
      {
      }

      std::string m_pathToMesh;

      std::shared_ptr<Graphics::IMaterial> m_material;

      virtual ~InstancedMeshComponentData() {}
   };
}