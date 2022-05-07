#include "Character.h"

namespace EngineCore
{
    Character::Character(const int32_t id, const float xTextureCoord, const float yTextureCoord, const float xTexSize, const float yTexSize,
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

    int32_t Character::GetId() const
    {
        return mId;
    }

    float Character::GetxTextureCoord() const
    {
        return mXTextureCoord;
    }

    float Character::GetyTextureCoord() const
    {
        return mYTextureCoord;
    }

    float Character::GetXMaxTextureCoord() const
    {
        return mXMaxTextureCoord;
    }

    float Character::GetYMaxTextureCoord() const
    {
        return mYMaxTextureCoord;
    }

    float Character::GetxOffset() const
    {
        return mXOffset;
    }

    float Character::GetyOffset() const
    {
        return mYOffset;
    }

    float Character::GetSizeX() const
    {
        return mSizeX;
    }

    float Character::GetSizeY() const
    {
        return mSizeY;
    }

    float Character::GetxAdvance() const
    {
        return mXAdvance;
    }
}