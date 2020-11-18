#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"
#include "Core/GameCore/ACamera.h"

#include <string>
#include <glm/vec3.hpp>

namespace Game
{
	struct CharacterMovementComponentData : public ComponentData
	{
      CharacterMovementComponentData(const std::string& gameObjectName, const glm::vec3& launchDirection, const std::string& cameraName)
			: ComponentData(gameObjectName)
         , mCameraName(cameraName)
         , m_launchDirection(launchDirection)
		{
		}

      std::string mCameraName;
		glm::vec3 m_launchDirection;
	};

   struct MovementComponentData : public ComponentData
   {
      MovementComponentData(const std::string& gameObjectName, const std::string& scriptName)
         : ComponentData(gameObjectName)
         , mScriptName(scriptName)
      {
      }

      std::string mScriptName;
   };
}