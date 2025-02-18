#pragma once
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>

namespace IO {
class FolderManager {

private:
    std::unordered_map<std::string /*file name*/, std::string /*relative path to file from exe file*/> mFilesPathMap;
    std::unordered_map<std::string /*file name*/, std::string /*abs path to file from exe file*/> mAbsFilesPathMap;

    std::string m_pathToExe;

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

    std::string GetRelativePathToFile(const std::string& fileName) const;
    std::string GetPathToExeFile() const;
    std::string GetResPath() const;
    std::string GetShortResPath() const;

    std::string GetModelPath() const;
    std::string GetShadersPath() const;
    std::string GetShaderCommonPath() const;
    std::string GetCollisionPath() const;
    std::string GetTexturesPath() const;
    std::string GetConfigPath() const;
    std::string GetFontsPath() const;
    std::string GetAudioPath() const;

    std::string GetMaterialTexturesPath() const;
    std::string GetGrassTexturePath() const;
    std::string GetLandscapeTexturePath() const;
    std::string GetCubemapTexturePath() const;
    std::string GetNormalMapPath() const;
    std::string GetSpecularMapPath() const;
    std::string GetAlbedoTexturePath() const;
    std::string GetDistortionTexturePath() const;
    std::string GetPostprocessTexturePath() const;
    std::string GetEditorTexturePath() const;
    std::string GetMaskTexturePath() const;
    std::string GetUiTexturePath() const;
    std::string GetScriptPath() const;
    std::string GetMaterialPath() const;
    std::string GetTweenerPath() const;

    std::string GetShortModelPath() const;
    std::string GetShortShadersPath() const;
    std::string GetShortShaderCommonPath() const;
    std::string GetShortCollisionPath() const;
    std::string GetShortTexturesPath() const;
    std::string GetShortFontsPath() const;
    std::string GetShortAudioPath() const;
    std::string GetShortIniPath() const;
    std::string GetShortMaterialTexturesPath() const;
    std::string GetShortGrassTexturePath() const;
    std::string GetShortLandscapeTexturePath() const;
    std::string GetShortCubemapTexturePath() const;
    std::string GetShortNormalMapPath() const;
    std::string GetShortSpecularMapPath() const;
    std::string GetShortAlbedoTexturePath() const;
    std::string GetShortDistortionTexturePath() const;
    std::string GetShortPostprocessTexturePath() const;
    std::string GetShortEditorTexturePath() const;
    std::string GetShortMaskTexturePath() const;
    std::string GetShortUiTexturePath() const;
    std::string GetShortScriptPath() const;
    std::string GetShortMaterialPath() const;
    std::string GetShortTweenerPath() const;

    void CreateFilePathMap(const std::string& absolutePathToDirectory, const std::string& relativePathToDirectory);
    std::string GetDirectoryRelativePathByFileName(const std::string& fileName) const;
    std::string GetFileAbsPathByFileName(const std::string& fileName) const;
};
} // namespace IO
