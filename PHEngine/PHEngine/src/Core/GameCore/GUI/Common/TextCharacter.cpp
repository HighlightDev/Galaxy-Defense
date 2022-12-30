#include "TextCharacter.h"

namespace EngineCore
{
    TextCharacter::TextCharacter(const int32_t id, const float xTextureCoord, const float yTextureCoord, const float xTexSize, const float yTexSize,
                         const float xOffset, const float yOffset, const float sizeX, const float sizeY, const float xAdvance)
        : mId(id),
          mXTextureCoord(xTextureCoord),
          mYTextureCoord(yTextureCoord),
          mXOffset(xOffset),
          mYOffset(yOffset),
          mSizeX(sizeX),
          mSizeY(sizeY),
          mXMaxTextureCoord(xTexSize + xTextureCoord),
          mYMaxTextureCoord(yTexSize + yTextureCoord),
          mXAdvance(xAdvance)
    {
    }

    int32_t TextCharacter::GetId() const
    {
        return mId;
    }

    float TextCharacter::GetxTextureCoord() const
    {
        return mXTextureCoord;
    }

    float TextCharacter::GetyTextureCoord() const
    {
        return mYTextureCoord;
    }

    float TextCharacter::GetXMaxTextureCoord() const
    {
        return mXMaxTextureCoord;
    }

    float TextCharacter::GetYMaxTextureCoord() const
    {
        return mYMaxTextureCoord;
    }

    float TextCharacter::GetxOffset() const
    {
        return mXOffset;
    }

    float TextCharacter::GetyOffset() const
    {
        return mYOffset;
    }

    float TextCharacter::GetSizeX() const
    {
        return mSizeX;
    }

    float TextCharacter::GetSizeY() const
    {
        return mSizeY;
    }

    float TextCharacter::GetxAdvance() const
    {
        return mXAdvance;
    }
}