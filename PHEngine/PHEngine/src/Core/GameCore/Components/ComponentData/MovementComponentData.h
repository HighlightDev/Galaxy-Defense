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
      MovementComponentData(glm::vec3 launchVelocity, const std::string& cameraName)
			: ComponentData()
         , mCameraName(cameraName)
         , m_launchVelocity(launchVelocity)
		{
		}

      std::string mCameraName;
		glm::vec3 m_launchVelocity;
	};

}