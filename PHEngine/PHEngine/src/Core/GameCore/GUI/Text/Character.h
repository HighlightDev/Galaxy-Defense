#pragma once

#include <stdint.h>
#include <cstdint>

namespace EngineCore
{
    /**
     * Simple data structure class holding information about a certain glyph in the
     * font texture atlas. All sizes are for a font-size of 1.
     *
     */
    class Character
    {
        int mId;
        float mXTextureCoord;
        float mYTextureCoord;
        float mXMaxTextureCoord;
        float mYMaxTextureCoord;
        float mXOffset;
        float mYOffset;
        float mSizeX;
        float mSizeY;
        float mXAdvance;

    public:
        /**
         * @param id
         *            - the ASCII value of the character.
         * @param xTextureCoord
         *            - the x texture coordinate for the top left corner of the
         *            character in the texture atlas.
         * @param yTextureCoord
         *            - the y texture coordinate for the top left corner of the
         *            character in the texture atlas.
         * @param xTexSize
         *            - the width of the character in the texture atlas.
         * @param yTexSize
         *            - the height of the character in the texture atlas.
         * @param xOffset
         *            - the x distance from the curser to the left edge of the
         *            character's quad.
         * @param yOffset
         *            - the y distance from the curser to the top edge of the
         *            character's quad.
         * @param sizeX
         *            - the width of the character's quad in screen space.
         * @param sizeY
         *            - the height of the character's quad in screen space.
         * @param xAdvance
         *            - how far in pixels the cursor should advance after adding
         *            this character.
         */
        Character(const int32_t id,
                  const float xTextureCoord,
                  const float yTextureCoord,
                  const float xTexSize,
                  const float yTexSize,
                  const float xOffset,
                  const float yOffset,
                  const float sizeX,
                  const float sizeY,
                  const float xAdvance);

        int32_t GetId() const;

        float GetxTextureCoord() const;

        float GetyTextureCoord() const;

        float GetXMaxTextureCoord() const;

        float GetYMaxTextureCoord() const;

        float GetxOffset() const;

        float GetyOffset() const;

        float GetSizeX() const;

        float GetSizeY() const;

        float GetxAdvance() const;
    };
}
