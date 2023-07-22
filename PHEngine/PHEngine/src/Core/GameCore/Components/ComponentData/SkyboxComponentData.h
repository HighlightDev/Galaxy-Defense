#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"

#include <string>

namespace EngineCore
{
	struct SkyboxComponentData : public ComponentData
	{
		SkyboxComponentData(const std::string &gameObjectName,
							const glm::vec3 &scale,
							const std::shared_ptr<Graphics::IMaterial> &material)
			: ComponentData(gameObjectName),
			  m_scale(scale),
			  m_material(material)
		{
		}

		glm::vec3 m_scale;

		std::shared_ptr<Graphics::IMaterial> m_material;
	};

}