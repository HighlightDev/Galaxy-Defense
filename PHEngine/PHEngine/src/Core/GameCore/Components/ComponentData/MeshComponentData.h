#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentType.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

#include <string>
#include <glm/vec3.hpp>

namespace Game
{
	struct MeshComponentData : public ComponentData
	{
      MeshComponentData(const std::string& gameObjectName, const std::string& pathToMesh, const glm::vec3& translation,
         const glm::vec3& rotation, const glm::vec3& scale, const std::string& mLuaScriptRelPath, Graphics::IMaterial* material)
			: ComponentData(gameObjectName)
			, m_pathToMesh(pathToMesh)
			, m_translation(translation)
			, m_eulerRotationDegrees(rotation)
			, m_scale(scale)
         , m_luaScriptPath(mLuaScriptRelPath)
         , m_material(material)
		{
		}

		std::string m_pathToMesh;
		glm::vec3 m_translation;
		glm::vec3 m_eulerRotationDegrees;
		glm::vec3 m_scale;
      std::string m_luaScriptPath;

      Graphics::IMaterial* m_material;
	};

}