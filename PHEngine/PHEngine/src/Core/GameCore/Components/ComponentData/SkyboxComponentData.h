#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"

#include <string>

namespace Game
{
	struct SkyboxComponentData : public ComponentData
	{
		SkyboxComponentData(const glm::vec3& scale, std::shared_ptr<IMaterial> materialInstance)
			: ComponentData()
         , m_scale(scale)
         , m_materialInstance(materialInstance)
		{
		}

		virtual uint64_t GetType() const override {

			return SKYBOX_COMPONENT;
		}

      glm::vec3 m_scale;

      std::shared_ptr<IMaterial> m_materialInstance;
	};

}