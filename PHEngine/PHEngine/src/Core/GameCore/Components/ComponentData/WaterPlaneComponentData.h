#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"

#include <string>
#include <glm/vec3.hpp>

namespace EngineCore
{
	struct WaterPlaneComponentData : public ComponentData
	{
		WaterPlaneComponentData(const std::string &gameObjectName,
								const glm::vec3 &translation,
								const glm::vec3 &rotation,
								const glm::vec3 &scale,
								const std::shared_ptr<Graphics::IMaterial> &materialInstance)

			: ComponentData(gameObjectName),
			  m_translation(translation),
			  m_eulerRotationDegrees(rotation),
			  m_scale(scale),
			  m_material(materialInstance)
		{
		}

		glm::vec3 m_translation;
		glm::vec3 m_eulerRotationDegrees;
		glm::vec3 m_scale;

		std::shared_ptr<Graphics::IMaterial> m_material;
	};

}