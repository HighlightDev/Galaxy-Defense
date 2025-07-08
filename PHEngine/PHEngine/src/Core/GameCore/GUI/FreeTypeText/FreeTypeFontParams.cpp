#include "FreeTypeFontParams.h"

namespace EngineCore::GUI {
FreeTypeFontParams::FreeTypeFontParams(const std::string& fontDescriptorFile, const int32_t pixelSize)
    : FontDescriptorFile(fontDescriptorFile)
    , PixelSize(pixelSize)
{
}
} // namespace EngineCore::GUI