#pragma once

#include <stdint.h>

#include <cstdint>
#include <string>

namespace EngineCore::GUI {
struct FreeTypeFontParams {
    std::string FontName;
    int32_t PixelSize;

    explicit FreeTypeFontParams(const std::string& fontName, const int32_t pixelSize);

    friend struct std::hash<FreeTypeFontParams>;

    bool operator==(const FreeTypeFontParams& other) const
    {
        return this->FontName == other.FontName && this->PixelSize == other.PixelSize;
    }
};
} // namespace EngineCore::GUI

namespace std {
using namespace EngineCore::GUI;
template<>
struct hash<FreeTypeFontParams> {
    std::size_t operator()(const FreeTypeFontParams& k) const
    {
        return hash<std::string>()(k.FontName) ^ hash<int32_t>()(k.PixelSize);
    }
};
} // namespace std
