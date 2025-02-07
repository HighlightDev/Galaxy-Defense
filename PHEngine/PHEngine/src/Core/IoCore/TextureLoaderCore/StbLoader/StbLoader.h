#pragma once

#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/IoCore/TextureLoaderCore/TextureResourceInfo.h"

#include <stb/stb_image.h>

#include <cstddef>
#include <string>

namespace IO {
namespace Images {
namespace Stb {
class StbLoader {

    uint8_t* m_lastAllocatedMemory;

public:
    StbLoader();

    ~StbLoader();

    uint8_t* AllocateTextureMemoryFromFile(const std::string& pathToFile, TextureResourceInfo& out_params);

    void ReleaseTextureMemory();
};
} // namespace Stb
} // namespace Images
} // namespace IO
