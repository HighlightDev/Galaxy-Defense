#include "FreeTypeTextFieldProxy.h"

#include "Core/GameCore/GUI/Common/UniqueFontTextIdGenerator.h"

using namespace EngineCore;

namespace EngineCore::GUI {
FreeTypeTextFieldProxy::FreeTypeTextFieldProxy()
    : mTextFieldId(-1)
    , mPositionChunkOffset(0)
    , mPositionChunkSize(0)
    , mTextureCoordinatesChunkOffset(0)
    , mTextureCoordinatesChunkSize(0)
    , mVertexStart(0)
    , mVerticesCount(0)
    , mIsVisible(false)
    , mText()
    , mFontParams("", 42)
    , mColor()
    , mPosition()
    , mFontFlags(static_cast<int32_t>(eFontFlags::WordWrap | eFontFlags::LeftAligned))
    , mCreatedMeshTextWidthHeightNormalized()
{
}

std::shared_ptr<FreeTypeTextFieldProxy> FreeTypeTextFieldProxy::CreateTextFieldProxyInstance(
    const int32_t uniqueTextId,
    const eTextFieldProxyType textFieldType,
    const bool isVisible,
    const std::string& text,
    const std::string& fontFileName,
    const glm::vec2& position,
    const glm::vec3& color,
    const int32_t pixelSize,
    const int32_t fontFlags,
    const eTextHorizontalAlignmentType textHorizontalAlignment,
    const eTextVerticalAlignmentType textVericalAlignment,
    const glm::ivec2& lineWidthHeight)
{
    std::shared_ptr<FreeTypeTextFieldProxy> result = std::make_shared<FreeTypeTextFieldProxy>();
    result->mTextFieldProxyType = textFieldType;
    result->mTextFieldId = uniqueTextId;
    result->mIsVisible = isVisible;
    result->mText = text;
    result->mFontParams = FreeTypeFontParams(fontFileName, pixelSize);
    result->mColor = color;
    result->mPosition = position;
    result->mLineWidthHeight = lineWidthHeight;
    result->mFontFlags = fontFlags;
    result->mTextHorizontalAlignment = textHorizontalAlignment;
    result->mTextVerticalAlignment = textVericalAlignment;
    return result;
}

int32_t FreeTypeTextFieldProxy::GetTextFieldId() const
{
    return mTextFieldId;
}

eTextFieldProxyType FreeTypeTextFieldProxy::GetTextFieldProxyType() const
{
    return mTextFieldProxyType;
}

size_t FreeTypeTextFieldProxy::GetPositionChunkOffset() const
{
    return mPositionChunkOffset;
}

size_t FreeTypeTextFieldProxy::GetPositionChunkSize() const
{
    return mPositionChunkSize;
}

size_t FreeTypeTextFieldProxy::GetTextureCoordinatesChunkOffset() const
{
    return mTextureCoordinatesChunkOffset;
}

size_t FreeTypeTextFieldProxy::GetTextureCoordinatesChunkSize() const
{
    return mTextureCoordinatesChunkSize;
}

size_t FreeTypeTextFieldProxy::GetVertexStart() const
{
    return mVertexStart;
}

size_t FreeTypeTextFieldProxy::GetVerticesCount() const
{
    return mVerticesCount;
}

bool FreeTypeTextFieldProxy::IsVisible() const
{
    return mIsVisible;
}

std::string FreeTypeTextFieldProxy::GetText() const
{
    return mText;
}

std::string FreeTypeTextFieldProxy::GetFontFileName() const
{
    return mFontParams.FontName;
}

int32_t FreeTypeTextFieldProxy::GetFontFlags() const
{
    return mFontFlags;
}

glm::vec3 FreeTypeTextFieldProxy::GetColor() const
{
    return mColor;
}

glm::vec2 FreeTypeTextFieldProxy::GetPosition() const
{
    return mPosition;
}

glm::ivec2 FreeTypeTextFieldProxy::GetLineWidthHeight() const
{
    return mLineWidthHeight;
}

glm::vec2 FreeTypeTextFieldProxy::GetCreatedMeshTextWidthHeightNormalized() const
{
    return mCreatedMeshTextWidthHeightNormalized;
}

glm::ivec2 FreeTypeTextFieldProxy::GetCreatedMeshTextWidthHeightScreenSpace() const
{
    return mCreatedMeshTextWidthHeightScreenSpace;
}

void FreeTypeTextFieldProxy::SetPositionChunkOffset(const size_t positionChunkOffset)
{
    mPositionChunkOffset = positionChunkOffset;
}

void FreeTypeTextFieldProxy::SetPositionChunkSize(const size_t positionChunkSize)
{
    mPositionChunkSize = positionChunkSize;
}

void FreeTypeTextFieldProxy::SetTextureCoordinatesChunkOffset(const size_t textureCoordinatesChunkOffset)
{
    mTextureCoordinatesChunkOffset = textureCoordinatesChunkOffset;
}

void FreeTypeTextFieldProxy::SetTextureCoordinatesChunkSize(const size_t textureCoordinatesChunkSize)
{
    mTextureCoordinatesChunkSize = textureCoordinatesChunkSize;
}

void FreeTypeTextFieldProxy::SetVertexStart(const size_t vertexStart)
{
    mVertexStart = vertexStart;
}

void FreeTypeTextFieldProxy::SetVerticesCount(const size_t verticesCount)
{
    mVerticesCount = verticesCount;
}

void FreeTypeTextFieldProxy::SetIsVisible(const bool isVisible)
{
    mIsVisible = isVisible;
}

void FreeTypeTextFieldProxy::SetText(const std::string& text)
{
    mText = text;
}

void FreeTypeTextFieldProxy::SetFontFileName(const std::string& fontFileName)
{
    mFontParams.FontName = fontFileName;
}

void FreeTypeTextFieldProxy::SetFontSize(const int32_t pixelSize)
{
    mFontParams.PixelSize = pixelSize;
}

int32_t FreeTypeTextFieldProxy::GetFontSize() const
{
    return mFontParams.PixelSize;
}

void FreeTypeTextFieldProxy::SetColor(const glm::vec3& color)
{
    mColor = color;
}

void FreeTypeTextFieldProxy::SetPosition(const glm::vec2& position)
{
    mPosition = position;
}

void FreeTypeTextFieldProxy::SetLineWidthHeight(const glm::ivec2& widthHeight)
{
    mLineWidthHeight = widthHeight;
}

void FreeTypeTextFieldProxy::SetFontFlags(const int32_t fontFlags)
{
    mFontFlags = fontFlags;
}

void FreeTypeTextFieldProxy::SetCreatedMeshTextWidthHeightScreenSpace(const glm::ivec2& createdMeshTextWidthHeightScreenSpace)
{
    mCreatedMeshTextWidthHeightScreenSpace = createdMeshTextWidthHeightScreenSpace;
}

void FreeTypeTextFieldProxy::SetCreatedMeshTextWidthHeightNormalized(const glm::vec2& createdMeshTextWidthHeightTextureSpace)
{
    mCreatedMeshTextWidthHeightNormalized = createdMeshTextWidthHeightTextureSpace;
}

eTextHorizontalAlignmentType FreeTypeTextFieldProxy::GetTextHorizontalAlignment() const
{
    return mTextHorizontalAlignment;
}

void FreeTypeTextFieldProxy::SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment)
{
    mTextHorizontalAlignment = textHorizontalAlignment;
}

eTextVerticalAlignmentType FreeTypeTextFieldProxy::GetextVerticalAlignment() const
{
    return mTextVerticalAlignment;
}

void FreeTypeTextFieldProxy::SetTextVerticalAlignment(const eTextVerticalAlignmentType textVericalAlignment)
{
    mTextVerticalAlignment = textVericalAlignment;
}

FreeTypeFontParams FreeTypeTextFieldProxy::GetFontParams() const
{
    return mFontParams;
}
} // namespace EngineCore::GUI
