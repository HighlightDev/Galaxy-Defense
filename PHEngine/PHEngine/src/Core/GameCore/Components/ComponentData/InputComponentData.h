#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"

#include <string>
#include <glm/vec3.hpp>

namespace Game
{
	struct InputComponentData : public ComponentData
	{
      InputComponentData()

			: ComponentData()
		{
		}

		virtual uint64_t GetType() const override {

			return INPUT_COMPONENT;
		}
		
	};

}