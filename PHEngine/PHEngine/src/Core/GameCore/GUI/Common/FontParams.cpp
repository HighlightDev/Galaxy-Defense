#include "FontParams.h"

namespace EngineCore {
FontParams::FontParams(const std::string& fontName, const std::string& fontDescriptorFile, const std::string& fontTextureAtlas)
    : FontName(fontName)
    , FontDescriptorFile(fontDescriptorFile)
    , FontTextureAtlas(fontTextureAtlas)
{
}
} // namespace EngineCore