#pragma once
#include <string>
#include <memory>
#include <stdint.h>

#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

namespace Common
{

	class FolderManager
	{

	private:

		const std::string BINARY_FOLDER_NAME = "\\bin";

		std::string m_rootFolder;

		static std::shared_ptr<FolderManager> m_instance;

		std::string ConcatDirectoryBack(int32_t countChangeDirectoryBack);

		void BuildPathToFolders();

	public:

		FolderManager();

		~FolderManager();

		static std::shared_ptr<FolderManager> GetInstance()
		{
			if (m_instance == nullptr)
				m_instance = std::make_shared<FolderManager>();

			return m_instance;
		}

		const std::string GetRootPath() const;
		const std::string GetResPath() const;
		const std::string GetModelPath() const;
		const std::string GetShadersPath() const;
      const std::string GetShaderCommonPath() const;
		const std::string GetCollisionPath() const;
		const std::string GetTexturesPath() const;
		const std::string GetIniPath() const;

		const std::string GetGrassTexturePath() const;
		const std::string GetHeightMapsTexturePath() const;
		const std::string GetLandscapeTexturePath() const;
		const std::string GetCubemapTexturePath() const;
		const std::string GetNormalMapPath() const;
		const std::string GetSpecularMapPath() const;
		const std::string GetAlbedoTexturePath() const;
		const std::string GetDistortionTexturePath() const;
		const std::string GetPostprocessTexturePath() const;
		const std::string GetEditorTexturePath() const;
      const std::string GetPersistencyPath() const;
	};
}

