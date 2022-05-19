#include "FontParams.h"

namespace EngineCore
{
    std::string FontDescriptorFile;
    std::string FontTextureAtlas;

    FontParams::FontParams(const std::string &fontDescriptorFile,
                           const std::string &fontTextureAtlas)
        : FontDescriptorFile(fontDescriptorFile),
          FontTextureAtlas(fontTextureAtlas)
    {
    }
}