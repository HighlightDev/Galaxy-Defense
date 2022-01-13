#include "FolderManager.h"
#include "Core/CommonCore/Assertion.h"

#include <filesystem>

namespace IO
{
	std::shared_ptr<FolderManager> FolderManager::m_instance;

	FolderManager::FolderManager()
		: mFilesPathMap(), m_rootFolder("")
	{
	}

	FolderManager::~FolderManager()
	{
	}

	void FolderManager::BuildSystemPathToFolders()
	{
		// Root folder
		m_rootFolder = EngineUtility::GetExecutablePath();
		assert(m_rootFolder != "");
	}
/*
terminate called after throwing an instance of 'std::filesystem::__cxx11::filesystem_error'
  what():  filesystem error: recursive directory iterator cannot open directory: No such file or directory [/home/dzinoviev/MyProjects/phengine/PHEngine/build/TinyGame//home/dzinoviev/MyProjects/phengine/PHEngine/build/TinyGame/res\texture\albedo\]
*/
	void FolderManager::CreateFilePathMap(const std::string &pathToDir)
	{
		using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

		const std::string &absolutePath = pathToDir;

		for (const auto &dirEntry : recursive_directory_iterator(absolutePath))
		{
			const std::string &fileName = std::string(dirEntry.path().filename().string());
			assert(mFilesPathMap.count(fileName) == 0);
			mFilesPathMap[fileName] = pathToDir;
		}
	}

	std::string FolderManager::GetDirectoryRelativePathByFileName(const std::string &fileName) const
	{
		assert(mFilesPathMap.count(fileName));
		return mFilesPathMap.at(fileName) + fileName;
	}

	const std::string FolderManager::GetRootPath() const
	{
		assert(m_rootFolder != ""); // if assert has fired, maybe you forget to invoke BuildSystemPathToFolders
		return m_rootFolder;
	}

	const std::string FolderManager::GetResPath() const
	{
		return GetRootPath() + "res" + SLASH;
	}

	const std::string FolderManager::GetModelPath() const
	{
		return GetResPath() + "model" + SLASH;
	}

	const std::string FolderManager::GetShadersPath() const
	{
		return GetResPath() + "shaders" + SLASH;
	}

	const std::string FolderManager::GetShaderCommonPath() const
	{
		return GetShadersPath() + "common" + SLASH;
	}

	const std::string FolderManager::GetCollisionPath() const
	{
		return GetResPath() + "collision" + SLASH;
	}

	const std::string FolderManager::GetTexturesPath() const
	{
		return GetResPath() + "texture" + SLASH;
	}

	const std::string FolderManager::GetIniPath() const
	{
		return GetResPath() + "ini" + SLASH;
	}

	const std::string FolderManager::GetGrassTexturePath() const
	{
		return GetTexturesPath() + "grass" + SLASH;
	}

	const std::string FolderManager::GetLandscapeTexturePath() const
	{
		return GetTexturesPath() + "landscape" + SLASH;
	}

	const std::string FolderManager::GetMaterialTexturesPath() const
	{
		return GetTexturesPath() + "materials" + SLASH;
	}

	const std::string FolderManager::GetCubemapTexturePath() const
	{
		return GetTexturesPath() + "cubemap" + SLASH;
	}

	const std::string FolderManager::GetNormalMapPath() const
	{
		return GetTexturesPath() + "normalmap" + SLASH;
	}

	const std::string FolderManager::GetSpecularMapPath() const
	{
		return GetTexturesPath() + "specularmap" + SLASH;
	}

	const std::string FolderManager::GetAlbedoTexturePath() const
	{
		return GetTexturesPath() + "albedo" + SLASH;
	}

	const std::string FolderManager::GetDistortionTexturePath() const
	{
		return GetTexturesPath() + "distortion" + SLASH;
	}

	const std::string FolderManager::GetPostprocessTexturePath() const
	{
		return GetTexturesPath() + "postprocess" + SLASH;
	}

	const std::string FolderManager::GetEditorTexturePath() const
	{
		return GetTexturesPath() + "editor" + SLASH;
	}

	const std::string FolderManager::GetScriptPath() const
	{
		return GetResPath() + "scripts" + SLASH;
	}

	const std::string FolderManager::GetMaterialPath() const
	{
		return GetResPath() + "materials" + SLASH;
	}

	const std::string FolderManager::GetTweenerPath() const
	{
		return GetResPath() + "tweeners" + SLASH;
	}
}
