#pragma once

#include "ITexture.h"

#include <string>
#include <vector>

namespace Graphics {
namespace Texture {

class CubemapTexture : public ITexture {
    std::vector<TexParams> m_texParams;

public:
    CubemapTexture(const std::vector<std::string>& pathToTextures, const std::string& textureName);

    CubemapTexture(TexParams cubemapTexParams, const std::string& textureName);

    ~CubemapTexture() override = default;

    void BindTexture(uint32_t textureSlot) const override;

    void UnbindTexture(uint32_t textureSlot) const override;

    void CleanUp() override;

    uint32_t GetTextureDescriptor() const override;

    glm::ivec2 GetTextureResolution() const override;

    TexParams GetTextureParameters() const override;

    float GetTextureAspectRatio() const override;

    eTextureType GetTextureType() const override;

private:
    uint32_t CreateCubemapTexture(const std::vector<std::string>& pathToTextures);

    uint32_t CreateEmptyCubemapTexture();
};

} // namespace Texture
} // namespace Graphics
