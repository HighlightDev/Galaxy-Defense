#pragma once

#include <stdint.h>

#include <cstdint>
#include <string>

namespace EngineCore::GUI {
struct FreeTypeFontParams {
    std::string FontDescriptorFile;
    int32_t PixelSize;

    explicit FreeTypeFontParams(const std::string& fontDescriptorFile, const int32_t pixelSize);

    friend struct std::hash<FreeTypeFontParams>;

    bool operator==(const FreeTypeFontParams& other) const
    {
        return this->FontDescriptorFile == other.FontDescriptorFile && this->PixelSize == other.PixelSize;
    }
};
} // namespace EngineCore::GUI

namespace std {
using namespace EngineCore::GUI;
template<>
struct hash<FreeTypeFontParams> {
    std::size_t operator()(const FreeTypeFontParams& k) const
    {
        return hash<std::string>()(k.FontDescriptorFile) ^ hash<int32_t>()(k.PixelSize);
    }
};
} // namespace std
