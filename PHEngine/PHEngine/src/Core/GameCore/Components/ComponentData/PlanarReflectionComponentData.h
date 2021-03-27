#pragma once
#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include <string>
#include <glm/vec3.hpp>
#include <gl/glew.h>

namespace Game
{
   class ACamera;

   struct PlanarReflectionComponentData : public ComponentData
   {
      PlanarReflectionComponentData(const std::string& gameObjectName, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& reflectionPlane, ACamera* ownerCamera,
         const ::Graphics::ViewPortInfo& fboViewPortInfo)
         : ComponentData(gameObjectName)
         , m_translation(translation)
         , m_eulerRotationDegrees(rotation)
         , m_scale(scale)
         , m_reflectionPlane(reflectionPlane)
         , m_ownerCamera(ownerCamera)
         , m_fboViewPortInfo(fboViewPortInfo)
      {
      }

      glm::vec3 m_translation;
      glm::vec3 m_eulerRotationDegrees;
      glm::vec3 m_scale;
      glm::vec4 m_reflectionPlane;
      ACamera* m_ownerCamera;
      ::Graphics::ViewPortInfo m_fboViewPortInfo;
   };

}