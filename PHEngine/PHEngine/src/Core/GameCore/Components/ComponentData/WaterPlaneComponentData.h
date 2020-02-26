#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"

#include <string>
#include <glm/vec3.hpp>

namespace Game
{
	struct WaterPlaneComponentData : public ComponentData
	{
      WaterPlaneComponentData(glm::vec3&& translation, glm::vec3&& rotation, glm::vec3&& scale, std::shared_ptr<IMaterial> materialInstance)

			: ComponentData()
			, m_translation(std::move(translation))
			, m_rotation(std::move(rotation))
			, m_scale(std::move(scale))
         , m_materialInstance(materialInstance)
		{
		}

		virtual uint64_t GetType() const override {

			return WATER_PLANE_COMPONENT;
		}

		glm::vec3 m_translation;
		glm::vec3 m_rotation;
		glm::vec3 m_scale;
   
      std::shared_ptr<IMaterial> m_materialInstance;
	};

}