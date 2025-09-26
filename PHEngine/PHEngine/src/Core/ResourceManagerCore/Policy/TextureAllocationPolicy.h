#pragma once

#include "Core/GraphicsCore/Texture/CubemapTexture.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/GraphicsCore/Texture/Texture2d.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <memory>
#include <string>
#include <vector>

using namespace Graphics::Texture;

namespace Resources {
struct TextureAllocationPolicy {
    static std::shared_ptr<ITexture> AllocateMemory(const std::string& arg);

    static void DeallocateMemory(std::shared_ptr<ITexture> arg);

private:
    static std::shared_ptr<ITexture> LoadTexture2dFromFile(const std::string& arg, const std::string& name);

    static std::shared_ptr<ITexture> LoadTextureCubeFromFile(const std::vector<std::string>& pathToFiles, const std::string& name);
};

} // namespace Resources
