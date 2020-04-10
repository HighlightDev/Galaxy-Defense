#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"

#include <string>
#include <glm/vec3.hpp>

namespace Game
{
   struct SkeletalMeshComponentData : public ComponentData
   {
      SkeletalMeshComponentData(std::string&& pathToMesh, glm::vec3&& translation, glm::vec3&& rotation, glm::vec3&& scale, std::shared_ptr<IMaterial> material)

         : ComponentData()
         , m_pathToMesh(std::move(pathToMesh))
         , m_translation(std::move(translation))
         , m_eulerRotationDegrees(std::move(rotation))
         , m_scale(std::move(scale))
         , m_material(material)
      {
      }

      virtual uint64_t GetType() const override {

         return SKELETAL_MESH_COMPONENT;
      }

      std::string m_pathToMesh;
      glm::vec3 m_translation;
      glm::vec3 m_eulerRotationDegrees;
      glm::vec3 m_scale;

      std::shared_ptr<IMaterial> m_material;
   };

}