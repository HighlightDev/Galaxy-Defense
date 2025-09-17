#include "HudTextField.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/GUI/Common/UniqueFontTextIdGenerator.h"
#include "Core/GameCore/LoggerExtension.h"

namespace EngineCore {
HudTextField::HudTextField(
    const std::string& fontName,
    const int32_t fontSize,
    const std::string& text,
    const glm::vec3& color,
    const glm::vec2& position,
    const glm::ivec2& lineMaxWidthHeight,
    const eTextHorizontalAlignmentType textHorizontalAlignment,
    const eTextVerticalAlignmentType textVericalAlignment)
    : mTextFieldId(UniqueFontTextIdGenerator::GenerateUniqueFontTextId())
    , mIsVisible(true)
    , mFontName(fontName)
    , mFontSize(fontSize)
    , mText(text)
    , mColor(color)
    , mPosition(position)
    , mLineMaxWidthHeight(lineMaxWidthHeight)
    , mTextHorizontalAlignment(textHorizontalAlignment)
    , mTextVerticalAlignment(textVericalAlignment)
    , mNormalizedSize(0.0f, 0.0f)
{
}

HudTextField::HudTextField(
    const std::string& fontName,
    const int32_t fontSize,
    const glm::vec3& color,
    const glm::ivec2& lineMaxWidthHeight,
    const eTextHorizontalAlignmentType textHorizontalAlignment,
    const eTextVerticalAlignmentType textVericalAlignment)
    : mTextFieldId(UniqueFontTextIdGenerator::GenerateUniqueFontTextId())
    , mIsVisible(false)
    , mFontName(fontName)
    , mFontSize(fontSize)
    , mText()
    , mColor(color)
    , mPosition()
    , mLineMaxWidthHeight(lineMaxWidthHeight)
    , mTextHorizontalAlignment(textHorizontalAlignment)
    , mTextVerticalAlignment(textVericalAlignment)
{
}

std::string HudTextField::GetText() const
{
    return mText;
}

int32_t HudTextField::GetTextFieldId() const
{
    return mTextFieldId;
}

bool HudTextField::GetIsVisible() const
{
    return mIsVisible;
}

std::string HudTextField::GetFontName() const
{
    return mFontName;
}

int32_t HudTextField::GetFontSize() const
{
    return mFontSize;
}

glm::vec3 HudTextField::GetColor() const
{
    return mColor;
}

glm::vec2 HudTextField::GetPosition() const
{
    return mPosition;
}

glm::ivec2 HudTextField::GetLineMaxWidthHeight() const
{
    return mLineMaxWidthHeight;
}

eTextHorizontalAlignmentType HudTextField::GetTextHorizontalAlignment() const
{
    return mTextHorizontalAlignment;
}

eTextVerticalAlignmentType HudTextField::GetTextVerticalAlignment() const
{
    return mTextVerticalAlignment;
}

void HudTextField::SetText(const std::string& text)
{
    mText = text;
}

void HudTextField::SetVisibility(const bool isVisible)
{
    mIsVisible = isVisible;
}

void HudTextField::SetColor(const glm::vec3& color)
{
    mColor = color;
}

void HudTextField::SetPosition(const glm::vec2& position)
{
    mPosition = position;
}

void HudTextField::SetTextNormalizedSize(const glm::vec2& normalizedSize)
{
    mNormalizedSize = normalizedSize;
}

glm::vec2 HudTextField::GetNormalizedSize() const
{
    return mNormalizedSize;
}

void HudTextField::SetTextScreenSpaceSize(const glm::ivec2& screenSpaceSize)
{
    mScreenSpaceSize = screenSpaceSize;
}

glm::ivec2 HudTextField::GetTextScreenSpaceSize() const
{
    return mScreenSpaceSize;
}
} // namespace EngineCore