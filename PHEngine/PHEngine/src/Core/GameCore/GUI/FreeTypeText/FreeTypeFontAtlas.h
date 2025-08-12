#pragma once

#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

#include <freetype/freetype.h>
#include <ft2build.h>
#include <glm/vec2.hpp>

#include <memory>
#include <utility>

using namespace Graphics::OpenGL;
using namespace Graphics::Texture;

namespace EngineCore::GUI {
class FreeTypeFont;
class FreeTypeFontAtlas {
public:
    struct Character {
        float advanceX;
        float advanceY;

        float bitmapWidth;
        float bitmapHeight;

        float bitmapLeft;
        float bitmapTop;

        float xOffset;
    };

    inline int getAtlasWidth()
    {
        return mWidthHeightTexture.x;
    }
    inline int getAtlasHeight()
    {
        return mWidthHeightTexture.y;
    }
    inline Character* getCharInfo()
    {
        return mChars;
    }

    inline std::shared_ptr<FreeTypeFont> GetFontFace() const
    {
        return mFont;
    }

    std::shared_ptr<ITexture> GetFontTextureAtlas() const
    {
        return mFontTextureAtlas;
    }

private:
    std::shared_ptr<ITexture> mFontTextureAtlas;

    VertexArrayObject m_buffer;

    std::shared_ptr<FreeTypeFont> mFont;

    int32_t mFontSize;

    glm::ivec2 mWidthHeightTexture;

    Character mChars[128];

    FT_GlyphSlot mSlot;

private:
    void InitializeFontAtlas();

public:
    FreeTypeFontAtlas(const VertexArrayObject& vao, std::shared_ptr<FreeTypeFont> font, const int32_t pixelSize);

    bool operator==(const FreeTypeFontAtlas& right) const;

    VertexArrayObject* GetBuffer();

    virtual void CleanUp();
};

} // namespace EngineCore::GUI
