#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"

#include <string>
#include <glm/vec3.hpp>

namespace Game
{
   struct SkeletalMeshComponentData : public ComponentData
   {
      SkeletalMeshComponentData(std::string&& pathToMesh, glm::vec3&& translation, glm::vec3&& rotation, glm::vec3&& scale,
         std::string&& vsPath, std::string&& fsPath, std::shared_ptr<IMaterial> material)

         : ComponentData()
         , m_pathToMesh(std::move(pathToMesh))
         , m_translation(std::move(translation))
         , m_rotation(std::move(rotation))
         , m_scale(std::move(scale))
         , m_vsShaderPath(std::move(vsPath))
         , m_fsShaderPath(std::move(fsPath))
         , m_material(material)
      {
      }

      virtual uint64_t GetType() const override {

         return SKELETAL_MESH_COMPONENT;
      }

      std::string m_pathToMesh;
      glm::vec3 m_translation;
      glm::vec3 m_rotation;
      glm::vec3 m_scale;
      std::string m_vsShaderPath;
      std::string m_fsShaderPath;

      std::shared_ptr<IMaterial> m_material;
   };

}