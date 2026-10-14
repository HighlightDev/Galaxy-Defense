#include "FreeTypeFontParams.h"

namespace EngineCore::GUI {
FreeTypeFontParams::FreeTypeFontParams(const std::string& fontName, const int32_t pixelSize)
    : FontName(fontName)
    , PixelSize(pixelSize)
{
}
} // namespace EngineCore::GUI