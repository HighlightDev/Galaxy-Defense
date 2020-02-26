#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"

#include <string>
#include <glm/vec3.hpp>
#include <gl/glew.h>

namespace Game
{
   struct CubemapComponentData : public ComponentData
   {
      CubemapComponentData(glm::vec3&& translation, glm::vec3&& rotation, glm::vec3&& scale,
         std::string&& vsPath, std::string&& fsPath, const LazyTextureAtlasObtainer& textureObtainer)

         : ComponentData()
         , m_translation(std::move(translation))
         , m_rotation(std::move(rotation))
         , m_scale(std::move(scale))
         , m_vsShaderPath(std::move(vsPath))
         , m_fsShaderPath(std::move(fsPath))
         , m_textureObtainer(textureObtainer)
      {
      }

      virtual uint64_t GetType() const override {

         return PRIMITIVE_COMPONENT;
      }

      glm::vec3 m_translation;
      glm::vec3 m_rotation;
      glm::vec3 m_scale;
      std::string m_vsShaderPath;
      std::string m_fsShaderPath;
      LazyTextureAtlasObtainer m_textureObtainer;
   };

}