#include "TextureAllocationPolicy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

#include <stdexcept>

using namespace EngineCore;

namespace Resources {
std::shared_ptr<ITexture> TextureAllocationPolicy::AllocateMemory(const std::string& arg)
{
    std::vector<std::string> pathToTextures = EngineUtility::Split(arg, ',');

    switch (pathToTextures.size()) {
    case 1:
        return LoadTexture2dFromFile(arg, arg);
    case 6:
        return LoadTextureCubeFromFile(pathToTextures, "cubemap_" + arg);
    default:
        ext_assert(false, "Undefined count of files: " + std::to_string(pathToTextures.size()) + " for texture: " + arg);
    }
    return nullptr;
}

void TextureAllocationPolicy::DeallocateMemory(std::shared_ptr<ITexture> arg)
{
    LogInfo("TextureAllocationPolicy::DeallocateMemory: ", arg->GetTextureName());
    arg->CleanUp();
}

std::shared_ptr<ITexture> TextureAllocationPolicy::LoadTexture2dFromFile(const std::string& arg, const std::string& name)
{
    LogInfo("TextureAllocationPolicy::LoadTexture2dFromFile: ", arg);
    return std::make_shared<Texture2d>(arg, new TextureAnisotropy(16.0f), name);
}

std::shared_ptr<ITexture>
TextureAllocationPolicy::LoadTextureCubeFromFile(const std::vector<std::string>& pathToFiles, const std::string& name)
{
    LogInfo("TextureAllocationPolicy::LoadTextureCubeFromFile: ", pathToFiles[0], " ...");
    std::vector<std::string> absolutePaths;

    for (auto it = pathToFiles.begin(); it != pathToFiles.end(); ++it) {
        absolutePaths.emplace_back(*it);
    }

    return std::make_shared<CubemapTexture>(absolutePaths, name);
}
} // namespace Resources
