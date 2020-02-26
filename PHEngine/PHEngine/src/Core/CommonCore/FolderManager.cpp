#include "FolderManager.h"

namespace Common
{

	std::shared_ptr<FolderManager> FolderManager::m_instance;

	FolderManager::FolderManager()
	{
		BuildPathToFolders();
	}


	FolderManager::~FolderManager()
	{
	}

	std::string FolderManager::ConcatDirectoryBack(int32_t countChangeDirectoryBack)
	{
		std::string resultStr = std::move(std::string(""));
		while (countChangeDirectoryBack-- > 0)
		{
			resultStr += "..\\";
		}
		return resultStr + "PHEngine\\";
	}

	void FolderManager::BuildPathToFolders()
	{
		// Root folder
		std::string currentDirPath = std::move(EngineUtility::GetExecutablePath());
		size_t indexOfRootEntrance = EngineUtility::IndexOf(currentDirPath, BINARY_FOLDER_NAME);
		std::string pathFromRootToCurrentDir = currentDirPath.substr(indexOfRootEntrance);

		size_t countChangeDirectoryBack = 0;
		for (auto& item : pathFromRootToCurrentDir) {
			countChangeDirectoryBack = item == '\\' ? ++countChangeDirectoryBack : countChangeDirectoryBack;
		}

		m_rootFolder = std::move(ConcatDirectoryBack(countChangeDirectoryBack));
	}

   const std::string FolderManager::GetRootPath() const
	{
		return m_rootFolder;
	}

   const std::string FolderManager::GetResPath() const
	{
		return GetRootPath() + "res\\";
	}

   const std::string FolderManager::GetModelPath() const
	{
		return GetResPath() + "model\\";
	}

	const std::string FolderManager::GetShadersPath() const
	{
		return GetResPath() + "shaders\\";
	}

   const std::string FolderManager::GetShaderCommonPath() const
   {
      return GetShadersPath() + "common\\";
   }

   const std::string FolderManager::GetCollisionPath() const
	{
		return GetResPath() + "collision\\";
	}

   const std::string FolderManager::GetTexturesPath() const
	{
		return GetResPath() + "texture\\";
	}

   const std::string FolderManager::GetIniPath() const
	{
		return GetResPath() + "ini\\";
	}

   const std::string FolderManager::GetGrassTexturePath() const
	{
		return GetTexturesPath() + "grass\\";
	}

   const std::string FolderManager::GetHeightMapsTexturePath() const
	{
		return GetTexturesPath() + "heightmaps\\";
	}

   const std::string FolderManager::GetLandscapeTexturePath() const
	{
		return GetTexturesPath() + "landscape\\";
	}

   const std::string FolderManager::GetCubemapTexturePath() const
	{
		return GetTexturesPath() + "cubemap\\";
	}

   const std::string FolderManager::GetNormalMapPath() const
	{
		return GetTexturesPath() + "normalmap\\";
	}

   const std::string FolderManager::GetSpecularMapPath() const
	{
		return GetTexturesPath() + "specularmap\\";
	}

   const std::string FolderManager::GetAlbedoTexturePath() const
	{
		return GetTexturesPath() + "albedo\\";
	}

   const std::string FolderManager::GetDistortionTexturePath() const
	{
		return GetTexturesPath() + "distortion\\";
	}

   const std::string FolderManager::GetPostprocessTexturePath() const
	{
		return GetTexturesPath() + "postprocess\\";
	}

   const std::string FolderManager::GetEditorTexturePath() const
	{
		return GetTexturesPath() + "editor\\";
	}
	
   const std::string FolderManager::GetPersistencyPath() const
   {
      return GetResPath() + "persistency\\";
   }
}
