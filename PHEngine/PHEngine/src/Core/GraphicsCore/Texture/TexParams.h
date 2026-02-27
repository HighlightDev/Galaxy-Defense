#pragma once

#include <gl/glew.h>

#include <cstddef>
#include <functional>
#include <string>

namespace Graphics {
namespace Texture {

struct TexParams {

    int32_t TexBufferWidth;
    int32_t TexBufferHeight;
    int32_t TexTarget;
    int32_t TexMagFilter;
    int32_t TexMinFilter;
    int32_t TexMipLvl;
    int32_t TexPixelInternalFormat;
    int32_t TexPixelFormat;
    int32_t TexPixelType;
    int32_t TexWrapMode;
    bool bIsGrayscale;

    bool bIsUniqueResource;

    TexParams(
        int32_t texBufferWidth,
        int32_t texBufferHeight,
        int32_t texTarget = GL_TEXTURE_2D,
        int32_t magFilter = GL_LINEAR,
        int32_t minFilter = GL_LINEAR,
        int32_t texMipLvl = 0,
        int32_t texPixelInternalFormat = GL_RGB,
        int32_t texPixelFormat = GL_RGB,
        int32_t texPixelType = GL_UNSIGNED_BYTE,
        int32_t texWrapMode = GL_REPEAT,
        bool bUnique_resource = false);

    TexParams();

    ~TexParams();

    static std::string ToString(const TexParams& texParams)
    {
        const std::unordered_map<GLint, std::string> internalFormatToStrMap
            = {{GL_RG, "RG"},
               {GL_RGB, "RGB"},
               {GL_RGBA, "RGBA"},
               {GL_RGB8, "RGB8"},
               {GL_RGBA8, "RGBA8"},
               {GL_RGB16, "RGB16"},
               {GL_RGB16F, "RGB16F"},
               {GL_RGBA16, "RGBA16"},
               {GL_DEPTH24_STENCIL8, "DEPTH24_STENCIL8"},
               {GL_DEPTH32F_STENCIL8, "DEPTH32F_STENCIL8"},
               {GL_DEPTH_COMPONENT24, "DEPTH_COMPONENT24"},
               {GL_RG_INTEGER, "RG_INTEGER"},
               {GL_R8, "R8"},
               {GL_R16, "R16"},
               {GL_RG8, "RG8"},
               {GL_RG16, "RG16"},
               {GL_R16F, "R16F"},
               {GL_R32F, "R32F"},
               {GL_RG16F, "RG16F"},
               {GL_RG32F, "RG32F"},
               {GL_R8I, "R8I"},
               {GL_R8UI, "R8UI"},
               {GL_R16I, "R16I"},
               {GL_R16UI, "R16UI"},
               {GL_R32I, "R32I"},
               {GL_R32UI, "R32UI"},
               {GL_RG8I, "RG8I"},
               {GL_RG8UI, "RG8UI"},
               {GL_RG16I, "RG16I"},
               {GL_RG16UI, "RG16UI"},
               {GL_RG32I, "RG32I"},
               {GL_RG32UI, "RG32UI"}};

        std::string result;
        result += std::to_string(texParams.TexBufferWidth) + "x" + std::to_string(texParams.TexBufferHeight) + "_";
        result += internalFormatToStrMap.count(texParams.TexPixelInternalFormat) > 0
            ? internalFormatToStrMap.at(texParams.TexPixelInternalFormat)
            : "unknown internal format";
        return result;
    }

private:
    friend struct std::hash<TexParams>;

    static size_t unique_index;

    size_t UniqueIndex;

public:
    bool operator==(const TexParams& other) const
    {
        return this->TexBufferWidth == other.TexBufferWidth && this->TexBufferHeight == other.TexBufferHeight
            && this->TexTarget == other.TexTarget && this->TexMagFilter == other.TexMagFilter
            && this->TexMinFilter == other.TexMinFilter && this->TexMipLvl == other.TexMipLvl
            && this->TexPixelInternalFormat == other.TexPixelInternalFormat && this->TexPixelFormat == other.TexPixelFormat
            && this->TexPixelType == other.TexPixelType && this->TexWrapMode == other.TexWrapMode
            && this->bIsGrayscale == other.bIsGrayscale
            && ((this->bIsUniqueResource || other.bIsUniqueResource) ? this->UniqueIndex == other.UniqueIndex : true);
    }
};
} // namespace Texture
} // namespace Graphics

namespace std {
using namespace Graphics::Texture;
template<>
struct hash<TexParams> {
    std::size_t operator()(const TexParams& k) const
    {
        return hash<size_t>()(k.UniqueIndex) ^ hash<int32_t>()(k.TexBufferWidth) ^ hash<int32_t>()(k.TexBufferHeight)
            ^ hash<int32_t>()(k.TexTarget) ^ hash<int32_t>()(k.TexMagFilter) ^ hash<int32_t>()(k.TexMinFilter)
            ^ hash<int32_t>()(k.TexMipLvl) ^ hash<int32_t>()(k.TexPixelInternalFormat) ^ hash<int32_t>()(k.TexPixelFormat)
            ^ hash<int32_t>()(k.TexPixelType) ^ hash<int32_t>()(k.TexWrapMode);
    }
};
} // namespace std
