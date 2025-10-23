#pragma once

#include "Core/GameCore/GUI/Common/TextFieldProxyType.h"
#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFontParams.h"

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

    FreeTypeFontParams mFontParams;

    glm::vec3 mColor;

    glm::vec2 mPosition;

    int32_t mFontFlags;

    glm::ivec2 mLineWidthHeight;

    glm::vec2 mCreatedMeshTextWidthHeightNormalized;

    glm::ivec2 mCreatedMeshTextWidthHeightScreenSpace;

    eTextHorizontalAlignmentType mTextHorizontalAlignment{eTextHorizontalAlignmentType::LEFT};

    eTextVerticalAlignmentType mTextVerticalAlignment{eTextVerticalAlignmentType::TOP};

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
        const eTextHorizontalAlignmentType textHorizontalAlignmentType,
        const eTextVerticalAlignmentType textVericalAlignmentType,
        const glm::ivec2& lineWidthHeight);

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

    glm::ivec2 GetLineWidthHeight() const;

    int32_t GetFontFlags() const;

    eTextHorizontalAlignmentType GetTextHorizontalAlignment() const;

    eTextVerticalAlignmentType GetextVerticalAlignment() const;

    glm::vec2 GetCreatedMeshTextWidthHeightNormalized() const;

    glm::ivec2 GetCreatedMeshTextWidthHeightScreenSpace() const;

    void SetPositionChunkOffset(const size_t positionChunkOffset);

    void SetPositionChunkSize(const size_t positionChunkSize);

    void SetTextureCoordinatesChunkOffset(const size_t textureCoordinatesChunkOffset);

    void SetTextureCoordinatesChunkSize(const size_t textureCoordinatesChunkSize);

    void SetVertexStart(const size_t vertexStart);

    void SetVerticesCount(const size_t verticesCount);

    void SetIsVisible(const bool isVisible);

    void SetText(const std::string& text);

    void SetFontFileName(const std::string& fontFileName);

    void SetFontSize(const int32_t pixelSize);

    void SetColor(const glm::vec3& color);

    void SetPosition(const glm::vec2& position);

    void SetLineWidthHeight(const glm::ivec2& widthHeight);

    void SetFontFlags(const int32_t fontFlags);

    FreeTypeFontParams GetFontParams() const;

    void SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment);

    void SetTextVerticalAlignment(const eTextVerticalAlignmentType textVericalAlignment);

    void SetCreatedMeshTextWidthHeightNormalized(const glm::vec2& createdMeshTextWidthHeightTextureSpace);

    void SetCreatedMeshTextWidthHeightScreenSpace(const glm::ivec2& createdMeshTextWidthHeightScreenSpace);
};
} // namespace EngineCore::GUI