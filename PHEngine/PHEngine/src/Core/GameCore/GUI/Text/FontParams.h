#pragma once

#include <string>
#include <stdint.h>
#include <cstdint>

namespace EngineCore
{
    struct FontParams
    {
        std::string FontDescriptorFile;
        std::string FontTextureAtlas;

        explicit FontParams(const std::string &fontDescriptorFile,
                            const std::string &fontTextureAtlas);

        friend struct std::hash<FontParams>;

        bool operator==(const FontParams &other) const
        {

            return this->FontDescriptorFile == other.FontDescriptorFile &&
                   this->FontTextureAtlas == other.FontTextureAtlas;
        }
    };
}

namespace std
{
    using namespace EngineCore;
    template <>
    struct hash<FontParams>
    {
        std::size_t operator()(const FontParams &k) const
        {
            return hash<std::string>()(k.FontDescriptorFile) ^
                   hash<std::string>()(k.FontTextureAtlas);
        }
    };
}
