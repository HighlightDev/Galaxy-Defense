#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"

#include <string>

namespace Game
{
	struct SkyboxComponentData : public ComponentData
	{
		SkyboxComponentData(const std::string& gameObjectName, const glm::vec3& scale, IMaterial* materialInstance)
			: ComponentData(gameObjectName)
         , m_scale(scale)
         , m_materialInstance(materialInstance)
		{
		}

      glm::vec3 m_scale;

      std::shared_ptr<IMaterial> m_materialInstance;
	};

}