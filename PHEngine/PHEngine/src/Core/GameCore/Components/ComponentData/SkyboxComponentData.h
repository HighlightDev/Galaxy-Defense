#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"

#include <string>

namespace Game
{
	struct SkyboxComponentData : public ComponentData
	{
		SkyboxComponentData(const std::string& gameObjectName, const glm::vec3& scale, IMaterial* material)
			: ComponentData(gameObjectName)
         , m_scale(scale)
         , m_material(material)
		{
		}

      glm::vec3 m_scale;

      Graphics::IMaterial* m_material;
	};

}