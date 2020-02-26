#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

#include <string>
#include <glm/vec3.hpp>

namespace Game
{
	struct StaticMeshComponentData : public ComponentData
	{
		StaticMeshComponentData(const std::string& pathToMesh, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale, std::shared_ptr<IMaterial> material)
			: ComponentData()
			, m_pathToMesh(pathToMesh)
			, m_translation(translation)
			, m_rotation(rotation)
			, m_scale(scale)
         , m_material(material)
		{
		}

		virtual uint64_t GetType() const override {

			return STATIC_MESH_COMPONENT;
		}

		std::string m_pathToMesh;
		glm::vec3 m_translation;
		glm::vec3 m_rotation;
		glm::vec3 m_scale;

      std::shared_ptr<IMaterial> m_material;
	};

}