#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>

namespace Graphics
{
    class TextFieldProxy
    {
    public:
        size_t mTextFieldId;

        // Chunk Data Begin
        size_t mPositionChunkOffset;
        size_t mPositionChunkSize;

        size_t mTextureCoordinatesChunkOffset;
        size_t mTextureCoordinatesChunkSize;

        size_t mOffsetChunkOffset;
        size_t mOffsetChunkSize;

        size_t mColorChunkOffset;
        size_t mColorChunkSize;
        // Chunk Data End

        std::string mText;

        std::string mFontName;

        float mFontSize;

        glm::vec3 mColor;

        glm::vec2 mPosition;

        float mLineMaxSize;

        int32_t mNumberOfLines;

        bool mIsCenteredText;
    };
}