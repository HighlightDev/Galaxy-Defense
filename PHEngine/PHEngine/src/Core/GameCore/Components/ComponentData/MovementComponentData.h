#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/ACamera.h"

#include <string>
#include <glm/vec3.hpp>

namespace EngineCore
{
   struct MovementComponentData : public ComponentData
   {
      MovementComponentData(const std::string& gameObjectName, const glm::vec3& launchDirection)
      : ComponentData(gameObjectName)
      , m_launchDirection(launchDirection)
      {
      } 

      glm::vec3 m_launchDirection;
   };

	struct HumanoidMovementComponentData : public MovementComponentData
	{
      HumanoidMovementComponentData(const std::string& gameObjectName, const glm::vec3& launchDirection, const std::string& cameraName)
			: MovementComponentData(gameObjectName, launchDirection)
         , mCameraName(cameraName)
		{
		}

      std::string mCameraName;
	};

   struct PlatformTraverseComponentData : public ComponentData
   {
      PlatformTraverseComponentData(const std::string& gameObjectName, const std::string& scriptName)
         : ComponentData(gameObjectName)
         , mScriptName(scriptName)
      {
      }

      std::string mScriptName;
   };
}