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

        size_t mChunkOffset;

        size_t mChunkSize;

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