#pragma once
#include <string>
#include <memory>
#include <cstddef>
#include <unordered_map>

#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

namespace IO
{

	class FolderManager
	{

	private:
		std::unordered_map<std::string /*file name*/, std::string /*full path to file*/> mFilesPathMap;

		std::string m_rootFolder;

		static std::shared_ptr<FolderManager> m_instance;

	public:
		FolderManager();

		~FolderManager();

		static std::shared_ptr<FolderManager> GetInstance()
		{
			if (m_instance == nullptr)
				m_instance = std::make_shared<FolderManager>();

			return m_instance;
		}

		void BuildSystemPathToFolders();
		const std::string GetRootPath() const;
		const std::string GetResPath() const;
		const std::string GetModelPath() const;
		const std::string GetShadersPath() const;
		const std::string GetShaderCommonPath() const;
		const std::string GetCollisionPath() const;
		const std::string GetTexturesPath() const;
		const std::string GetIniPath() const;

		const std::string GetMaterialTexturesPath() const;
		const std::string GetGrassTexturePath() const;
		const std::string GetLandscapeTexturePath() const;
		const std::string GetCubemapTexturePath() const;
		const std::string GetNormalMapPath() const;
		const std::string GetSpecularMapPath() const;
		const std::string GetAlbedoTexturePath() const;
		const std::string GetDistortionTexturePath() const;
		const std::string GetPostprocessTexturePath() const;
		const std::string GetEditorTexturePath() const;
		const std::string GetPersistencyPath() const;
		const std::string GetScriptPath() const;
		const std::string GetMaterialPath() const;
		const std::string GetTweenerPath() const;

		void CreateFilePathMap(const std::string &pathToDir);
		std::string GetDirectoryRelativePathByFileName(const std::string &fileName) const;
	};
}
