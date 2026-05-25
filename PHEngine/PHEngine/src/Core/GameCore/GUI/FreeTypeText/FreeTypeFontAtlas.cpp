#include "FreeTypeFontAtlas.h"

#include "Core/GraphicsCore/Texture/Texture2d.h"
#include "FreeTypeFlags.h"
#include "FreeTypeFont.h"

#include <gl/glew.h>

namespace EngineCore::GUI {
FreeTypeFontAtlas::FreeTypeFontAtlas(
    const VertexArrayObject& vao, std::shared_ptr<FreeTypeFont> font, const FreeTypeFontParams& fontParams)
    : m_buffer(vao)
    , mFont(font)
    , mFontParams(fontParams)
    , mWidthHeightTexture(0, 0)
{
    InitializeFontAtlas();
}

bool FreeTypeFontAtlas::operator==(const FreeTypeFontAtlas& right) const
{
    return this->m_buffer.GetDescriptor() == right.m_buffer.GetDescriptor();
}

VertexArrayObject* FreeTypeFontAtlas::GetBuffer()
{
    return &m_buffer;
}

void FreeTypeFontAtlas::CleanUp()
{
    m_buffer.CleanUp();
    mFontTextureAtlas->CleanUp();
    mFont->CleanUp();
}

void FreeTypeFontAtlas::InitializeFontAtlas()
{
    LogInfo(
        "FreeTypeFontAtlas::InitializeFontAtlas: Initializing font atlas for font '",
        mFont->GetFontFileName(),
        "' with size ",
        mFontParams.PixelSize);

    const auto face = mFont->getFaceHandle();
    mSlot = face->glyph;
    FT_Set_Pixel_Sizes(
        face, // Font face handle
        0, // Pixel width  (0 defaults to pixel height)
        mFontParams.PixelSize); // Pixel height (0 defaults to pixel width)

    // Map language codes to sets of UTF-8 character codes
    const std::unordered_map<std::string, std::vector<uint32_t>>& languageCharMap = FreeTypeFont::getLanguageCharMap();

    for (const auto& [languageName, symbols] : languageCharMap) {
        for (const int32_t c : symbols) {
            if (FT_Load_Char(face, c, eFTLoadFlags::LoadRender)) {
                LogInfo("Loading character ", c, " failed!");
                continue; // try next character
            }

            mWidthHeightTexture.x += mSlot->bitmap.width + 2; // add the width of this glyph to our texture width
            mWidthHeightTexture.y = std::max(mWidthHeightTexture.y, (int)mSlot->bitmap.rows);
        }
    }

    GLuint texID;
    // Create texture
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0 + texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Create an empty texture with the correct dimensions
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mWidthHeightTexture.x, mWidthHeightTexture.y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

    int texPos = 0; // texture offset

    // Padding pixels are written with a zero buffer — passing NULL to glTexSubImage2D
    // with non-zero dimensions is UB and causes driver-level hangs.
    const std::vector<uint8_t> zeroPadding(mWidthHeightTexture.y, 0);

    for (const auto& [languageName, symbols] : languageCharMap) {
        for (const int32_t c : symbols) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                continue;

            // Layout per character slot (width = bitmap.width + 2):
            //   texPos + 0                     : 1px zero padding (left)
            //   texPos + 1                     : bitmap.width px glyph data
            //   texPos + 1 + bitmap.width      : 1px zero padding (right)
            glTexSubImage2D(
                GL_TEXTURE_2D, 0, texPos, 0, 1, mWidthHeightTexture.y, GL_RED, GL_UNSIGNED_BYTE, zeroPadding.data()); // left
                                                                                                                      // padding

            if (mSlot->bitmap.width > 0 && mSlot->bitmap.rows > 0) {
                glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    texPos + 1,
                    0,
                    mSlot->bitmap.width,
                    mSlot->bitmap.rows,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    mSlot->bitmap.buffer); // glyph
            }

            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                texPos + 1 + (int)mSlot->bitmap.width,
                0,
                1,
                mWidthHeightTexture.y,
                GL_RED,
                GL_UNSIGNED_BYTE,
                zeroPadding.data()); // right padding

            // Store glyph info in our char array for this pixel size
            mChars[c].advanceX = mSlot->advance.x >> 6;
            mChars[c].advanceY = mSlot->advance.y >> 6;

            mChars[c].bitmapWidth = mSlot->bitmap.width;
            mChars[c].bitmapHeight = mSlot->bitmap.rows;

            mChars[c].bitmapLeft = mSlot->bitmap_left;
            mChars[c].bitmapTop = mSlot->bitmap_top;

            // xOffset points to the glyph start (after left padding)
            mChars[c].xOffset = (float)(texPos + 1) / (float)mWidthHeightTexture.x;

            // Increase texture offset
            texPos += mSlot->bitmap.width + 2;
        }
    }

    mFontTextureAtlas = std::make_shared<Texture2d>(texID, mWidthHeightTexture, "font_" + mFont->GetFontFileName());
}

} // namespace EngineCore::GUI
