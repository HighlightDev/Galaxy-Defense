#pragma once

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlas.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlasCell.h"

#include <glm/vec4.hpp>

#include <memory>

using namespace Graphics::Texture;

namespace Graphics {
struct ITextureAtlasBuildedNotify {
    virtual void NotifyTextureAtlasBuilded() = 0;
};

struct TextureAtlasHandler : public ITextureAtlasBuildedNotify {
protected:
    std::shared_ptr<ITexture> m_atlasResource;

    eTextureType m_texType;

public:
    TextureAtlasHandler(const std::shared_ptr<ITexture>& resource)
        : m_atlasResource(resource)
        , m_texType(eTextureType::UNDEFINED)
    {
    }

    inline std::shared_ptr<ITexture> GetAtlasResource() const
    {
        return m_atlasResource;
    }

    inline eTextureType GetTextureType() const
    {
        return m_texType;
    }
};

struct Texture2dAtlasHandler : public TextureAtlasHandler {
protected:
    TextureAtlasCell m_atlasCell;

    glm::vec4 mTextureAtlasOffset;

public:
    Texture2dAtlasHandler(const std::shared_ptr<ITexture>& resource, const TextureAtlasCell& cell)
        : TextureAtlasHandler(resource)
        , m_atlasCell(cell)
    {
        m_texType = eTextureType::TEXTURE_2D;
    }

    inline TextureAtlasCell GetAtlasCell() const
    {
        return m_atlasCell;
    }

    glm::vec4 GetTextureAtlasOffset() const
    {

        return mTextureAtlasOffset;
    }

    void NotifyTextureAtlasBuilded() override
    {
        CalculatePosOffsetShadowMapAtlas();
    }

private:
    void CalculatePosOffsetShadowMapAtlas()
    {

        auto texAtlasCell = GetAtlasCell();

        float x = static_cast<float>(texAtlasCell.X);
        float y = static_cast<float>(texAtlasCell.Y);
        float width = static_cast<float>(texAtlasCell.Width);
        float height = static_cast<float>(texAtlasCell.Height);

        float invShadowMapWidth = 1.0f / static_cast<float>(texAtlasCell.TotalShadowMapWidth);
        float invShadowMapHeight = 1.0f / static_cast<float>(texAtlasCell.TotalShadowMapHeight);

        float x_start = x * invShadowMapWidth;
        float y_start = y * invShadowMapHeight;
        float x_offset = width * invShadowMapWidth;
        float y_offset = height * invShadowMapHeight;

        mTextureAtlasOffset = glm::vec4(x_start, y_start, x_offset, y_offset);
    }
};

struct TextureCubeAtlasHandler : public TextureAtlasHandler {
    TextureCubeAtlasHandler(const std::shared_ptr<ITexture>& resource)
        : TextureAtlasHandler(resource)
    {
        m_texType = eTextureType::TEXTURE_CUBE;
    }

    void NotifyTextureAtlasBuilded() override
    {
    }
};
} // namespace Graphics
