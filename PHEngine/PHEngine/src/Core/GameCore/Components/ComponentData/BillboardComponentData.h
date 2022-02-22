#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"

#include <string>
#include <glm/vec3.hpp>

namespace EngineCore
{
   struct BillboardComponentData : public ComponentData
   {
      BillboardComponentData(const std::string& gameObjectName, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale,
         const std::string& vsPath, const std::string& fsPath, const std::string& gsPath, const std::string&& pathToTexture)
         : ComponentData(gameObjectName)
         , m_translation(translation)
         , m_eulerRotationDegrees(rotation)
         , m_scale(scale)
         , m_pathToTexture(pathToTexture)
         , m_vsShaderPath(vsPath)
         , m_fsShaderPath(fsPath)
         , m_gsShaderPath(gsPath)
      {
      }

      glm::vec3 m_translation;
      glm::vec3 m_eulerRotationDegrees;
      glm::vec3 m_scale;
      std::string m_pathToTexture;
      std::string m_vsShaderPath;
      std::string m_fsShaderPath;
      std::string m_gsShaderPath;
   };

}