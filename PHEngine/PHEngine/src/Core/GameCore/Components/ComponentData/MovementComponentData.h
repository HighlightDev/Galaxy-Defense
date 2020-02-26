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
      MovementComponentData(glm::vec3 launchVelocity, ICamera* const camera)
			: ComponentData()
         , m_camera(camera)
         , m_launchVelocity(launchVelocity)
		{
		}

		virtual uint64_t GetType() const override {

			return MOVEMENT_COMPONENT;
		}

      ICamera* const m_camera;
		glm::vec3 m_launchVelocity;
	};

}