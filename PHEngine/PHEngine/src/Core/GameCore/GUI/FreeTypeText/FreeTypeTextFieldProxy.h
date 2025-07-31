#pragma once

#include "Core/GameCore/GUI/Common/TextFieldProxyType.h"
#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <string>

using namespace EngineCore;
using namespace Graphics;

namespace EngineCore::GUI {
class FreeTypeTextFieldProxy {
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

    std::string mFontFileName;

    int32_t mPixelSize;

    glm::vec3 mColor;

    glm::vec2 mPosition;

    int32_t mFontFlags;

    int32_t mLineWidth;

    int32_t mLineHeight;

    int32_t mCreatedMeshTextWidth;

    int32_t mCreatedMeshTextHeight;

    bool mIsSubscribedOnTextScreenSpaceSizeUpdate;

public:
    explicit FreeTypeTextFieldProxy();

    static std::shared_ptr<FreeTypeTextFieldProxy> CreateTextFieldProxyInstance(
        const int32_t uniqueTextId,
        const eTextFieldProxyType textFieldProxyType,
        const bool isVisible,
        const std::string& text,
        const std::string& fontFileName,
        const glm::vec2& position,
        const glm::vec3& color,
        const int32_t pixelSize,
        const int32_t fontFlags,
        const int32_t lineWidth,
        const int32_t lineHeight,
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

    std::string GetFontFileName() const;

    int32_t GetFontSize() const;

    glm::vec3 GetColor() const;

    glm::vec2 GetPosition() const;

    int32_t GetLineHeight() const;

    int32_t GetLineWidth() const;

    int32_t GetFontFlags() const;

    int32_t GetCreatedMeshTextWidth() const;

    int32_t GetCreatedMeshTextHeight() const;

    bool GetIsSubscribedOnTextScreenSpaceSizeUpdate() const;

    void SetPositionChunkOffset(const size_t positionChunkOffset);

    void SetPositionChunkSize(const size_t positionChunkSize);

    void SetTextureCoordinatesChunkOffset(const size_t textureCoordinatesChunkOffset);

    void SetTextureCoordinatesChunkSize(const size_t textureCoordinatesChunkSize);

    void SetVertexStart(const size_t vertexStart);

    void SetVerticesCount(const size_t verticesCount);

    void SetIsVisible(const bool isVisible);

    void SetText(const std::string& text);

    void SetFontFileName(const std::string& fontFileName);

    void SetPixelSize(const int32_t pixelSize);

    void SetColor(const glm::vec3& color);

    void SetPosition(const glm::vec2& position);

    void SetLineWidth(const int32_t lineWidth);

    void SetLineHeight(const int32_t lineHeight);

    void SetFontFlags(const int32_t fontFlags);

    void SetCreatedMeshTextWidth(const int32_t createdMeshTextWidth);

    void SetCreatedMeshTextHeight(const int32_t createdMeshTextHeight);

    void SetIsSubscribedOnTextScreenSpaceSizeUpdate(const bool isSubscribedOnTextScreenSpaceSizeUpdate);
};
} // namespace EngineCore::GUI