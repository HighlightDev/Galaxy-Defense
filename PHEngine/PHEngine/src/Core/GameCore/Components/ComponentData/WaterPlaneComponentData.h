#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"

#include <string>
#include <glm/vec3.hpp>

namespace Game
{
	struct WaterPlaneComponentData : public ComponentData
	{
      WaterPlaneComponentData(const std::string& gameObjectName, glm::vec3&& translation, glm::vec3&& rotation, glm::vec3&& scale, Graphics::IMaterial* materialInstance)

			: ComponentData(gameObjectName)
			, m_translation(std::move(translation))
			, m_eulerRotationDegrees(std::move(rotation))
			, m_scale(std::move(scale))
         , m_material(materialInstance)
		{
		}

		glm::vec3 m_translation;
		glm::vec3 m_eulerRotationDegrees;
		glm::vec3 m_scale;
   
      Graphics::IMaterial* m_material;
	};

}