#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <memory>

#include "TextFieldProxyType.h"

namespace Graphics
{
    class TextFieldProxy
    {
        eTextFieldProxyType mTextFieldProxyType{eTextFieldProxyType::UNDEFINED};

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

        float mLineMaxWidth;

        int32_t mNumberOfLines;

        bool mIsCenteredText;

        float mCreatedMeshTextWidth;

        float mCreatedMeshTextHeight;

        bool mIsSubscribedOnTextScreenSpaceSizeUpdate;

    public:
        TextFieldProxy();

        static std::shared_ptr<TextFieldProxy> CreateTextFieldProxyInstance(
            const int32_t uniqueTextId,
            const eTextFieldProxyType textFieldProxyType,
            const bool isVisible,
            const std::string &text,
            const std::string &fontName,
            const glm::vec2 &position,
            const glm::vec3 &color,
            const float fontSize,
            const bool isCenteredText,
            const float lineMaxWidth,
            const int32_t numberOfLines,
            const bool isSubscribedOnTextScreenSpaceSizeUpdate);

        int32_t GetTextFieldId() const;

        eTextFieldProxyType GetTextFieldProxyType() const;

        size_t GetPositionChunkOffset() const;

        size_t GetPositionChunkSize() const;

        size_t GetTextureCoordinatesChunkOffset() const;

        size_t GetTextureCoordinatesChunkSize() const;

        size_t GetVertexStart() const;

        size_t GetVerticesCount() const;

        bool GetIsVisible() const;

        std::string GetText() const;

        std::string GetFontName() const;

        float GetFontSize() const;

        glm::vec3 GetColor() const;

        glm::vec2 GetPosition() const;

        float GetLineMaxWidth() const;

        int32_t GetNumberOfLines() const;

        bool GetIsCenteredText() const;

        float GetCreatedMeshTextWidth() const;

        float GetCreatedMeshTextHeight() const;

        bool GetIsSubscribedOnTextScreenSpaceSizeUpdate() const;

        void SetPositionChunkOffset(const size_t positionChunkOffset);

        void SetPositionChunkSize(const size_t positionChunkSize);

        void SetTextureCoordinatesChunkOffset(const size_t textureCoordinatesChunkOffset);

        void SetTextureCoordinatesChunkSize(const size_t textureCoordinatesChunkSize);

        void SetVertexStart(const size_t vertexStart);

        void SetVerticesCount(const size_t verticesCount);

        void SetIsVisible(const bool isVisible);

        void SetText(const std::string &text);

        void SetFontName(const std::string &fontName);

        void SetFontSize(const float fontSize);

        void SetColor(const glm::vec3 &color);

        void SetPosition(const glm::vec2 &position);

        void SetLineMaxWidth(const float lineMaxWidth);

        void SetNumberOfLines(const int32_t numberOfLines);

        void SetIsCenteredText(const bool isCenteredText);

        void SetCreatedMeshTextWidth(const float createdMeshTextWidth);

        void SetCreatedMeshTextHeight(const float createdMeshTextHeight);

        void SetIsSubscribedOnTextScreenSpaceSizeUpdate(const bool isSubscribedOnTextScreenSpaceSizeUpdate);
    };
}