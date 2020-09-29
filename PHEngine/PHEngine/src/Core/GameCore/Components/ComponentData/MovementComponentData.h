#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"
#include "Core/GameCore/ICamera.h"

#include <string>
#include <glm/vec3.hpp>

namespace Game
{
	struct MovementComponentData : public ComponentData
	{
      MovementComponentData(const std::string& gameObjectName, const glm::vec3& launchDirection, const std::string& cameraName)
			: ComponentData(gameObjectName)
         , mCameraName(cameraName)
         , m_launchDirection(launchDirection)
		{
		}

      std::string mCameraName;
		glm::vec3 m_launchDirection;
	};

}