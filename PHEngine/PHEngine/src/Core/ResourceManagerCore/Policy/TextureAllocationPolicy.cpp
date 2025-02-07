#include "TextureAllocationPolicy.h"

#include "Core/GameCore/LoggerExtension.h"

#include <stdexcept>

using namespace EngineCore;

namespace Resources {
std::shared_ptr<ITexture> TextureAllocationPolicy::AllocateMemory(const std::string& arg)
{
    LogInfo("TextureAllocationPolicy::AllocateMemory: ", arg);
    std::vector<std::string> pathToTextures = EngineUtility::Split(arg, ',');

    switch (pathToTextures.size()) {
    case 1:
        return LoadTexture2dFromFile(arg);
    case 6:
        return LoadTextureCubeFromFile(pathToTextures);
    default:
        throw std::invalid_argument("Undefined count of files.");
    }
}

void TextureAllocationPolicy::DeallocateMemory(std::shared_ptr<ITexture> arg)
{
    LogInfo("TextureAllocationPolicy::DeallocateMemory: ", arg->GetTextureDescriptor());
    arg->CleanUp();
}

std::shared_ptr<ITexture> TextureAllocationPolicy::LoadTexture2dFromFile(const std::string& arg)
{
    return std::make_shared<Texture2d>(arg, new TextureAnisotropy(16.0f));
}

std::shared_ptr<ITexture> TextureAllocationPolicy::LoadTextureCubeFromFile(const std::vector<std::string>& pathToFiles)
{
    std::vector<std::string> absolutePaths;

    for (auto it = pathToFiles.begin(); it != pathToFiles.end(); ++it) {
        absolutePaths.emplace_back(*it);
    }

    return std::make_shared<CubemapTexture>(absolutePaths);
}
} // namespace Resources
