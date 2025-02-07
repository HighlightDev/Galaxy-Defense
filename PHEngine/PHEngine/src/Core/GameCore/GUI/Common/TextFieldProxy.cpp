#include "TextFieldProxy.h"

#include "UniqueFontTextIdGenerator.h"

using namespace EngineCore;

namespace Graphics {
TextFieldProxy::TextFieldProxy()
    : mTextFieldId(-1)
    , mPositionChunkOffset(0)
    , mPositionChunkSize(0)
    , mTextureCoordinatesChunkOffset(0)
    , mTextureCoordinatesChunkSize(0)
    , mVertexStart(0)
    , mVerticesCount(0)
    , mIsVisible(false)
    , mText()
    , mFontName()
    , mFontSize(0.0f)
    , mColor()
    , mPosition()
    , mLineMaxWidth(0.0f)
    , mNumberOfLines(0)
    , mTextHorizontalAlignment(eTextHorizontalAlignmentType::LEFT)
    , mCreatedMeshTextWidth(0.0f)
    , mCreatedMeshTextHeight(0.0f)
    , mIsSubscribedOnTextScreenSpaceSizeUpdate(false)
{
}

std::shared_ptr<TextFieldProxy> TextFieldProxy::CreateTextFieldProxyInstance(
    const int32_t uniqueTextId,
    const eTextFieldProxyType textFieldType,
    const bool isVisible,
    const std::string& text,
    const std::string& fontName,
    const glm::vec2& position,
    const glm::vec3& color,
    const float fontSize,
    const eTextHorizontalAlignmentType textHorizontalAlignment,
    const float lineMaxWidth,
    const int32_t numberOfLines,
    const bool isSubscribedOnTextScreenSpaceSizeUpdate)
{
    std::shared_ptr<TextFieldProxy> result = std::make_shared<TextFieldProxy>();
    result->mTextFieldProxyType = textFieldType;
    result->mTextFieldId = uniqueTextId;
    result->mIsVisible = isVisible;
    result->mText = text;
    result->mFontName = fontName;
    result->mFontSize = fontSize;
    result->mColor = color;
    result->mPosition = position;
    result->mLineMaxWidth = lineMaxWidth;
    result->mNumberOfLines = numberOfLines;
    result->mTextHorizontalAlignment = textHorizontalAlignment;
    result->mIsSubscribedOnTextScreenSpaceSizeUpdate = isSubscribedOnTextScreenSpaceSizeUpdate;
    return result;
}

int32_t TextFieldProxy::GetTextFieldId() const
{
    return mTextFieldId;
}

eTextFieldProxyType TextFieldProxy::GetTextFieldProxyType() const
{
    return mTextFieldProxyType;
}

size_t TextFieldProxy::GetPositionChunkOffset() const
{
    return mPositionChunkOffset;
}

size_t TextFieldProxy::GetPositionChunkSize() const
{
    return mPositionChunkSize;
}

size_t TextFieldProxy::GetTextureCoordinatesChunkOffset() const
{
    return mTextureCoordinatesChunkOffset;
}

size_t TextFieldProxy::GetTextureCoordinatesChunkSize() const
{
    return mTextureCoordinatesChunkSize;
}

size_t TextFieldProxy::GetVertexStart() const
{
    return mVertexStart;
}

size_t TextFieldProxy::GetVerticesCount() const
{
    return mVerticesCount;
}

bool TextFieldProxy::GetIsVisible() const
{
    return mIsVisible;
}

std::string TextFieldProxy::GetText() const
{
    return mText;
}

std::string TextFieldProxy::GetFontName() const
{
    return mFontName;
}

float TextFieldProxy::GetFontSize() const
{
    return mFontSize;
}

glm::vec3 TextFieldProxy::GetColor() const
{
    return mColor;
}

glm::vec2 TextFieldProxy::GetPosition() const
{
    return mPosition;
}

float TextFieldProxy::GetLineMaxWidth() const
{
    return mLineMaxWidth;
}

int32_t TextFieldProxy::GetNumberOfLines() const
{
    return mNumberOfLines;
}

eTextHorizontalAlignmentType TextFieldProxy::GetTextHorizontalAlignment() const
{
    return mTextHorizontalAlignment;
}

float TextFieldProxy::GetCreatedMeshTextWidth() const
{
    return mCreatedMeshTextWidth;
}

float TextFieldProxy::GetCreatedMeshTextHeight() const
{
    return mCreatedMeshTextHeight;
}

bool TextFieldProxy::GetIsSubscribedOnTextScreenSpaceSizeUpdate() const
{
    return mIsSubscribedOnTextScreenSpaceSizeUpdate;
}

void TextFieldProxy::SetPositionChunkOffset(const size_t positionChunkOffset)
{
    mPositionChunkOffset = positionChunkOffset;
}

void TextFieldProxy::SetPositionChunkSize(const size_t positionChunkSize)
{
    mPositionChunkSize = positionChunkSize;
}

void TextFieldProxy::SetTextureCoordinatesChunkOffset(const size_t textureCoordinatesChunkOffset)
{
    mTextureCoordinatesChunkOffset = textureCoordinatesChunkOffset;
}

void TextFieldProxy::SetTextureCoordinatesChunkSize(const size_t textureCoordinatesChunkSize)
{
    mTextureCoordinatesChunkSize = textureCoordinatesChunkSize;
}

void TextFieldProxy::SetVertexStart(const size_t vertexStart)
{
    mVertexStart = vertexStart;
}

void TextFieldProxy::SetVerticesCount(const size_t verticesCount)
{
    mVerticesCount = verticesCount;
}

void TextFieldProxy::SetIsVisible(const bool isVisible)
{
    mIsVisible = isVisible;
}

void TextFieldProxy::SetText(const std::string& text)
{
    mText = text;
}

void TextFieldProxy::SetFontName(const std::string& fontName)
{
    mFontName = fontName;
}

void TextFieldProxy::SetFontSize(const float fontSize)
{
    mFontSize = fontSize;
}

void TextFieldProxy::SetColor(const glm::vec3& color)
{
    mColor = color;
}

void TextFieldProxy::SetPosition(const glm::vec2& position)
{
    mPosition = position;
}

void TextFieldProxy::SetLineMaxWidth(const float lineMaxWidth)
{
    mLineMaxWidth = lineMaxWidth;
}

void TextFieldProxy::SetNumberOfLines(const int32_t numberOfLines)
{
    mNumberOfLines = numberOfLines;
}

void TextFieldProxy::SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment)
{
    mTextHorizontalAlignment = textHorizontalAlignment;
}

void TextFieldProxy::SetCreatedMeshTextWidth(const float createdMeshTextWidth)
{
    mCreatedMeshTextWidth = createdMeshTextWidth;
}

void TextFieldProxy::SetCreatedMeshTextHeight(const float createdMeshTextHeight)
{
    mCreatedMeshTextHeight = createdMeshTextHeight;
}

void TextFieldProxy::SetIsSubscribedOnTextScreenSpaceSizeUpdate(const bool isSubscribedOnTextScreenSpaceSizeUpdate)
{
    mIsSubscribedOnTextScreenSpaceSizeUpdate = isSubscribedOnTextScreenSpaceSizeUpdate;
}
} // namespace Graphics
