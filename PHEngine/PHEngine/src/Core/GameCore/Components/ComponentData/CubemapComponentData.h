#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"

#include <string>
#include <glm/vec3.hpp>
#include <gl/glew.h>

namespace Game
{
   struct CubemapComponentData : public ComponentData
   {
      CubemapComponentData(const std::string& gameObjectName, glm::vec3&& translation, glm::vec3&& rotation, glm::vec3&& scale,
         std::string&& vsPath, std::string&& fsPath, const TextureAtlasSpaceRequest& textureObtainer)

         : ComponentData(gameObjectName)
         , m_translation(std::move(translation))
         , m_eulerRotationDegrees(std::move(rotation))
         , m_scale(std::move(scale))
         , m_vsShaderPath(std::move(vsPath))
         , m_fsShaderPath(std::move(fsPath))
         , m_textureObtainer(textureObtainer)
      {
      }

      glm::vec3 m_translation;
      glm::vec3 m_eulerRotationDegrees;
      glm::vec3 m_scale;
      std::string m_vsShaderPath;
      std::string m_fsShaderPath;
      TextureAtlasSpaceRequest m_textureObtainer;
   };

}