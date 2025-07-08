#include "FreeTypeFontAtlas.h"

#include "FreeTypeFont.h"

#include <gl/glew.h>

namespace EngineCore::GUI {
FreeTypeFontAtlas::FreeTypeFontAtlas(const VertexArrayObject& vao, std::shared_ptr<FreeTypeFont> font, const int32_t pixelSize)
    : m_buffer(vao)
    , mFont(font)
    , mPixelSize(pixelSize)
    , mWidthHeightTexture(0, 0)
{
    InitializeFontAtlas();
}

FreeTypeFontAtlas::~FreeTypeFontAtlas()
{
    glDeleteTextures(1, &mTexID);
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
}

void FreeTypeFontAtlas::InitializeFontAtlas()
{
    const auto face = mFont->getFaceHandle();
    mSlot = face->glyph;
    FT_Set_Pixel_Sizes(
        face, // Font face handle
        0, // Pixel width  (0 defaults to pixel height)
        mPixelSize); // Pixel height (0 defaults to pixel width)

    // Main char set (32 - 128)
    for (int i = 32; i < 128; ++i) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            fprintf(stderr, "Loading character %c failed!\n", i);
            continue; // try next character
        }

        mWidthHeightTexture.x += mSlot->bitmap.width + 2; // add the width of this glyph to our texture width
        // Note: We add 2 pixels of blank space between glyphs for padding - this helps reduce texture bleeding
        //       that can occur with antialiasing

        mWidthHeightTexture.y = std::max(mWidthHeightTexture.y, (int)mSlot->bitmap.rows);
    }

    // Create texture
    glGenTextures(1, &mTexID);
    glActiveTexture(GL_TEXTURE0 + mTexID);
    glBindTexture(GL_TEXTURE_2D, mTexID);
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

    for (int i = 32; i < 128; ++i) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER))
            continue;

        // Add this character glyph to our texture
        glTexSubImage2D(GL_TEXTURE_2D, 0, texPos, 0, 1, mSlot->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, (char*)0); // padding
        glTexSubImage2D(
            GL_TEXTURE_2D, 0, texPos, 0, mSlot->bitmap.width, mSlot->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, mSlot->bitmap.buffer);
        glTexSubImage2D(GL_TEXTURE_2D, 0, texPos, 0, 1, mSlot->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, (char*)0); // padding

        // Store glyph info in our char array for this pixel size
        mChars[i].advanceX = mSlot->advance.x >> 6;
        mChars[i].advanceY = mSlot->advance.y >> 6;

        mChars[i].bitmapWidth = mSlot->bitmap.width;
        mChars[i].bitmapHeight = mSlot->bitmap.rows;

        mChars[i].bitmapLeft = mSlot->bitmap_left;
        mChars[i].bitmapTop = mSlot->bitmap_top;

        mChars[i].xOffset = (float)texPos / (float)mWidthHeightTexture.x;

        // Increase texture offset
        texPos += mSlot->bitmap.width + 2;
    }
}

} // namespace EngineCore::GUI
