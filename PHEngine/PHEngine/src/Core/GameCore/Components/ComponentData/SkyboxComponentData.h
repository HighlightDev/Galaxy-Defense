#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"

#include <string>

namespace Game
{
	struct SkyboxComponentData : public ComponentData
	{
		SkyboxComponentData(glm::vec3 scale, float rotateSpeed, std::string&& vsPath,
			std::string&& fsPath, std::string&& commaSeparatedPathToSixTexturesDay, std::string&& commaSeparatedPathToSixTexturesNight = "")

			: ComponentData()
			, m_rotateSpeed(rotateSpeed)
			, m_scale(scale)
			, m_vsShaderPath(std::move(vsPath))
			, m_fsShaderPath(std::move(fsPath))
			, m_commaSeparatedPathToSixTexturesDay(std::move(commaSeparatedPathToSixTexturesDay))
			, m_commaSeparatedPathToSixTexturesNight(std::move(commaSeparatedPathToSixTexturesNight))
		{

		}

		virtual uint64_t GetType() const override {

			return SKYBOX_COMPONENT;
		}

		float m_rotateSpeed;
      glm::vec3 m_scale;
		std::string m_commaSeparatedPathToSixTexturesDay;
		std::string m_commaSeparatedPathToSixTexturesNight;
		std::string m_vsShaderPath;
		std::string m_fsShaderPath;
	};

}