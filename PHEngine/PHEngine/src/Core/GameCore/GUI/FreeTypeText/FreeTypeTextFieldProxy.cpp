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
    , mLineWidth(0)
    , mLineHeight(0)
    , mFontFlags(static_cast<int32_t>(eFontFlags::WordWrap | eFontFlags::LeftAligned))
    , mCreatedMeshTextWidth(0.0f)
    , mCreatedMeshTextHeight(0.0f)
    , mIsSubscribedOnTextScreenSpaceSizeUpdate(false)
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
    const int32_t lineWidth,
    const int32_t lineHeight,
    const bool isSubscribedOnTextScreenSpaceSizeUpdate)
{
    std::shared_ptr<FreeTypeTextFieldProxy> result = std::make_shared<FreeTypeTextFieldProxy>();
    result->mTextFieldProxyType = textFieldType;
    result->mTextFieldId = uniqueTextId;
    result->mIsVisible = isVisible;
    result->mText = text;
    result->mFontParams = FreeTypeFontParams(fontFileName, pixelSize);
    result->mColor = color;
    result->mPosition = position;
    result->mLineWidth = lineWidth;
    result->mLineHeight = lineHeight;
    result->mFontFlags = fontFlags;
    result->mTextHorizontalAlignment = textHorizontalAlignment;
    result->mIsSubscribedOnTextScreenSpaceSizeUpdate = isSubscribedOnTextScreenSpaceSizeUpdate;
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

bool FreeTypeTextFieldProxy::GetIsVisible() const
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

int32_t FreeTypeTextFieldProxy::GetLineWidth() const
{
    return mLineWidth;
}

int32_t FreeTypeTextFieldProxy::GetLineHeight() const
{
    return mLineHeight;
}

float FreeTypeTextFieldProxy::GetCreatedMeshTextWidth() const
{
    return mCreatedMeshTextWidth;
}

float FreeTypeTextFieldProxy::GetCreatedMeshTextHeight() const
{
    return mCreatedMeshTextHeight;
}

bool FreeTypeTextFieldProxy::GetIsSubscribedOnTextScreenSpaceSizeUpdate() const
{
    return mIsSubscribedOnTextScreenSpaceSizeUpdate;
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

void FreeTypeTextFieldProxy::SetLineWidth(const int32_t lineWidth)
{
    mLineWidth = lineWidth;
}

void FreeTypeTextFieldProxy::SetLineHeight(const int32_t lineHeight)
{
    mLineHeight = lineHeight;
}

void FreeTypeTextFieldProxy::SetFontFlags(const int32_t fontFlags)
{
    mFontFlags = fontFlags;
}

void FreeTypeTextFieldProxy::SetCreatedMeshTextWidth(const float createdMeshTextWidth)
{
    mCreatedMeshTextWidth = createdMeshTextWidth;
}

void FreeTypeTextFieldProxy::SetCreatedMeshTextHeight(const float createdMeshTextHeight)
{
    mCreatedMeshTextHeight = createdMeshTextHeight;
}

void FreeTypeTextFieldProxy::SetIsSubscribedOnTextScreenSpaceSizeUpdate(const bool isSubscribedOnTextScreenSpaceSizeUpdate)
{
    mIsSubscribedOnTextScreenSpaceSizeUpdate = isSubscribedOnTextScreenSpaceSizeUpdate;
}

eTextHorizontalAlignmentType FreeTypeTextFieldProxy::GetTextHorizontalAlignment() const
{
    return mTextHorizontalAlignment;
}

void FreeTypeTextFieldProxy::SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment)
{
    mTextHorizontalAlignment = textHorizontalAlignment;
}

FreeTypeFontParams FreeTypeTextFieldProxy::GetFontParams() const
{
    return mFontParams;
}
} // namespace EngineCore::GUI
