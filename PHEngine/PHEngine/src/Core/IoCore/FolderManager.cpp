#include "FolderManager.h"
#include "Core/CommonCore/Assertion.h"

#include <filesystem>

namespace IO
{
	std::shared_ptr<FolderManager> FolderManager::m_instance;

	FolderManager::FolderManager()
		: mFilesPathMap(), m_pathToExe("")
	{
	}
      
	FolderManager::~FolderManager()
	{
	}

	void FolderManager::BuildSystemPathToFolders()
	{
		// Root folder
		m_pathToExe = EngineUtility::GetExecutablePath();
		assert(m_pathToExe != "");

		CreateFilePathMap(GetAlbedoTexturePath(), GetShortAlbedoTexturePath());
		CreateFilePathMap(GetNormalMapPath(), GetShortNormalMapPath());
		CreateFilePathMap(GetSpecularMapPath(), GetShortSpecularMapPath());
		CreateFilePathMap(GetDistortionTexturePath(), GetShortDistortionTexturePath());
		CreateFilePathMap(GetCubemapTexturePath(), GetShortCubemapTexturePath());
		CreateFilePathMap(GetMaterialTexturesPath(), GetShortMaterialTexturesPath());
		CreateFilePathMap(GetGrassTexturePath(), GetShortGrassTexturePath());
		CreateFilePathMap(GetModelPath(), GetShortModelPath());
		CreateFilePathMap(GetMaterialPath(), GetShortMaterialPath());
		CreateFilePathMap(GetScriptPath(), GetShortScriptPath());
		CreateFilePathMap(GetTweenerPath(), GetShortTweenerPath());
		CreateFilePathMap(GetFontsPath(), GetShortFontsPath());
		CreateFilePathMap(GetAudioPath(), GetShortAudioPath());
		CreateFilePathMap(GetMaskTexturePath(), GetShortMaskTexturePath());
	}

	void FolderManager::CreateFilePathMap(const std::string &absolutePathToDirectory, const std::string &relativePathToDirectory)
	{
		using directory_iterator = std::filesystem::directory_iterator;

		for (const auto &dirEntry : directory_iterator(absolutePathToDirectory))
		{
			const std::string &fileName = std::string(dirEntry.path().filename().string());
			assert(mFilesPathMap.count(fileName) == 0);
			mFilesPathMap[fileName] = relativePathToDirectory;
		}
	}

	std::string FolderManager::GetDirectoryRelativePathByFileName(const std::string &fileName) const
	{
		assert(mFilesPathMap.count(fileName));
		return mFilesPathMap.at(fileName) + fileName;
	}

	std::string FolderManager::GetPathToExeFile() const
	{
		assert(m_pathToExe != ""); // if assert has fired, maybe you forget to invoke BuildSystemPathToFolders
#ifdef _WIN32
      return m_pathToExe + SLASH;
#elif __linux__
      return m_pathToExe;
#endif
	}

	std::string FolderManager::GetResPath() const
	{
#ifdef _WIN32
		return GetPathToExeFile() + SLASH + "res" + SLASH;
#elif __linux__
      return GetPathToExeFile() + "res" + SLASH;
#endif
	}

	std::string FolderManager::GetShortResPath() const
	{
      std::string result = "res";
      return result + SLASH;
	}

	std::string FolderManager::GetShortModelPath() const
	{
		return GetShortResPath() + "model" + SLASH;
	}

	std::string FolderManager::GetShortShadersPath() const
	{
		return GetShortResPath() + "shaders" + SLASH;
	}

	std::string FolderManager::GetShortShaderCommonPath() const
	{
		return FolderManager::GetShortShadersPath() + "common" + SLASH;
	}

	std::string FolderManager::GetShortCollisionPath() const
	{
		return GetShortResPath() + "collision" + SLASH;
	}

	std::string FolderManager::GetShortTexturesPath() const
	{
		return GetShortResPath() + "texture" + SLASH;
	}

	std::string FolderManager::GetShortIniPath() const
	{
		return GetShortResPath() + "ini" + SLASH;
	}

	std::string FolderManager::GetShortMaterialTexturesPath() const
	{
		return GetShortTexturesPath() + "materials" + SLASH;
	}

	std::string FolderManager::GetShortGrassTexturePath() const
	{
		return GetShortTexturesPath() + "grass" + SLASH;
	}

	std::string FolderManager::GetShortLandscapeTexturePath() const
	{
		return GetShortTexturesPath() + "landscape" + SLASH;
	}

	std::string FolderManager::GetShortCubemapTexturePath() const
	{
		return GetShortTexturesPath() + "cubemap" + SLASH;
	}

	std::string FolderManager::GetShortNormalMapPath() const
	{
		return GetShortTexturesPath() + "normalmap" + SLASH;
	}

	std::string FolderManager::GetShortSpecularMapPath() const
	{
		return GetShortTexturesPath() + "specularmap" + SLASH;
	}

	std::string FolderManager::GetShortAlbedoTexturePath() const
	{
		return GetShortTexturesPath() + "albedo" + SLASH;
	}

	std::string FolderManager::GetShortDistortionTexturePath() const
	{
		return GetShortTexturesPath() + "distortion" + SLASH;
	}

	std::string FolderManager::GetShortPostprocessTexturePath() const
	{
		return GetShortTexturesPath() + "postprocess" + SLASH;
	}

	std::string FolderManager::GetShortEditorTexturePath() const
	{
		return GetShortTexturesPath() + "editor" + SLASH;
	}

	std::string FolderManager::GetShortMaskTexturePath() const
	{
		return GetShortTexturesPath() + "mask" + SLASH;
	}

	std::string FolderManager::GetShortScriptPath() const
	{
		return GetShortResPath() + "scripts" + SLASH;
	}

	std::string FolderManager::GetShortMaterialPath() const
	{
		return GetShortResPath() + "materials" + SLASH;
	}

	std::string FolderManager::GetShortFontsPath() const
	{
		return GetShortResPath() + "fonts" + SLASH;
	}

	std::string FolderManager::GetShortAudioPath() const
	{
		return GetShortResPath() + "audio" + SLASH;
	}

	std::string FolderManager::GetShortTweenerPath() const
	{
		return GetShortResPath() + "tweeners" + SLASH;
	}

	std::string FolderManager::GetModelPath() const
	{
		return GetResPath() + "model" + SLASH;
	}

	std::string FolderManager::GetShadersPath() const
	{
		return GetResPath() + "shaders" + SLASH;
	}

	std::string FolderManager::GetShaderCommonPath() const
	{
		return GetShadersPath() + "common" + SLASH;
	}

	std::string FolderManager::GetCollisionPath() const
	{
		return GetResPath() + "collision" + SLASH;
	}

	std::string FolderManager::GetTexturesPath() const
	{
		return GetResPath() + "texture" + SLASH;
	}

	std::string FolderManager::GetConfigPath() const
	{
		return GetResPath() + "config" + SLASH;
	}

	std::string FolderManager::GetFontsPath() const
	{
		return GetResPath() + "fonts" + SLASH;
	}

	std::string FolderManager::GetAudioPath() const
	{
		return GetResPath() + "audio" + SLASH;
	}

	std::string FolderManager::GetGrassTexturePath() const
	{
		return GetTexturesPath() + "grass" + SLASH;
	}

	std::string FolderManager::GetLandscapeTexturePath() const
	{
		return GetTexturesPath() + "landscape" + SLASH;
	}

	std::string FolderManager::GetMaterialTexturesPath() const
	{
		return GetTexturesPath() + "materials" + SLASH;
	}

	std::string FolderManager::GetCubemapTexturePath() const
	{
		return GetTexturesPath() + "cubemap" + SLASH;
	}

	std::string FolderManager::GetNormalMapPath() const
	{
		return GetTexturesPath() + "normalmap" + SLASH;
	}

	std::string FolderManager::GetSpecularMapPath() const
	{
		return GetTexturesPath() + "specularmap" + SLASH;
	}

	std::string FolderManager::GetAlbedoTexturePath() const
	{
		return GetTexturesPath() + "albedo" + SLASH;
	}

	std::string FolderManager::GetDistortionTexturePath() const
	{
		return GetTexturesPath() + "distortion" + SLASH;
	}

	std::string FolderManager::GetPostprocessTexturePath() const
	{
		return GetTexturesPath() + "postprocess" + SLASH;
	}

	std::string FolderManager::GetEditorTexturePath() const
	{
		return GetTexturesPath() + "editor" + SLASH;
	}

	std::string FolderManager::GetMaskTexturePath() const
	{
		return GetTexturesPath() + "mask" + SLASH;
	}

	std::string FolderManager::GetScriptPath() const
	{
		return GetResPath() + "scripts" + SLASH;
	}

	std::string FolderManager::GetMaterialPath() const
	{
		return GetResPath() + "materials" + SLASH;
	}

	std::string FolderManager::GetTweenerPath() const
	{
		return GetResPath() + "tweeners" + SLASH;
	}
}
