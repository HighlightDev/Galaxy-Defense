#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"

#include <string>
#include <glm/vec3.hpp>
#include <gl/glew.h>

namespace Game
{
   struct CubemapComponentData : public ComponentData
   {
      CubemapComponentData(const std::string &gameObjectName, const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale,
                           const std::string &vsPath, const std::string &fsPath, const TextureAtlasSpaceRequest &textureObtainer)

          : ComponentData(gameObjectName)
          , m_translation(translation)
          , m_eulerRotationDegrees(rotation)
          , m_scale(scale)
          , m_vsShaderPath(vsPath)
          , m_fsShaderPath(fsPath)
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