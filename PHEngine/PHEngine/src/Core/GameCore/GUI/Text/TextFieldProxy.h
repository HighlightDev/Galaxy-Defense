#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>

namespace Graphics
{
    class TextFieldProxy
    {
    public:
        TextFieldProxy()
            : mTextFieldId(-1),
              mPositionChunkOffset(0),
              mPositionChunkSize(0),
              mTextureCoordinatesChunkOffset(0),
              mTextureCoordinatesChunkSize(0),
              mVertexStart(0),
              mVerticesCount(0),
              mIsVisible(false),
              mText(),
              mFontName(),
              mFontSize(0.0f),
              mColor(),
              mPosition(),
              mLineMaxSize(0.0f),
              mNumberOfLines(0),
              mIsCenteredText(false),
              mCreatedMeshTextWidth(0.0f),
              mCreatedMeshTextHeight(0.0f),
              mIsSubscribedOnTextScreenSpaceSizeUpdate(false)
        {
        }

        int32_t mTextFieldId;

        // Chunk Data Begin
        size_t mPositionChunkOffset;
        size_t mPositionChunkSize;

        size_t mTextureCoordinatesChunkOffset;
        size_t mTextureCoordinatesChunkSize;
        // Chunk Data End

        size_t mVertexStart;
        size_t mVerticesCount;

        bool mIsVisible;

        std::string mText;

        std::string mFontName;

        float mFontSize;

        glm::vec3 mColor;

        glm::vec2 mPosition;

        float mLineMaxSize;

        int32_t mNumberOfLines;

        bool mIsCenteredText;

        float mCreatedMeshTextWidth;
        float mCreatedMeshTextHeight;

        bool mIsSubscribedOnTextScreenSpaceSizeUpdate;
    };
}