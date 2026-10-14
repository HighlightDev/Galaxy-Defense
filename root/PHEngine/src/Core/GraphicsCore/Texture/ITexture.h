#pragma once

#include "TexParams.h"

#include <glm/vec2.hpp>

#include <cstddef>

namespace Graphics {
namespace Texture {
enum class eTextureType { UNDEFINED, TEXTURE_2D, TEXTURE_CUBE };

class ITexture {
protected:
    uint32_t m_texDescriptor;

    std::string m_textureName;

public:
    explicit ITexture(const std::string& textureName);

    virtual ~ITexture() = default;

    bool operator==(const ITexture& right) const;

    virtual void BindTexture(uint32_t textureSlot) const = 0;
    virtual void UnbindTexture(uint32_t textureSlot) const = 0;
    virtual void CleanUp() = 0;
    virtual uint32_t GetTextureDescriptor() const = 0;
    virtual glm::ivec2 GetTextureResolution() const = 0;
    virtual TexParams GetTextureParameters() const = 0;
    virtual float GetTextureAspectRatio() const = 0;
    virtual eTextureType GetTextureType() const = 0;

    std::string GetTextureName() const;
};
} // namespace Texture
} // namespace Graphics
