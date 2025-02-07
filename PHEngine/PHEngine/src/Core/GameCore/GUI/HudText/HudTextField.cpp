#include "HudTextField.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/GUI/Common/UniqueFontTextIdGenerator.h"
#include "Core/GameCore/LoggerExtension.h"

namespace EngineCore {
HudTextField::HudTextField(
    const std::string& fontName,
    const float fontSize,
    const std::string& text,
    const glm::vec3& color,
    const glm::vec2& position,
    const float lineMaxSize,
    const int32_t numberOfLines,
    const eTextHorizontalAlignmentType textHorizontalAlignment)
    : mTextFieldId(UniqueFontTextIdGenerator::GenerateUniqueFontTextId())
    , mIsVisible(true)
    , mFontName(fontName)
    , mFontSize(fontSize)
    , mText(text)
    , mColor(color)
    , mPosition(position)
    , mLineMaxWidth(lineMaxSize)
    , mNumberOfLines(numberOfLines)
    , mTextHorizontalAlignment(textHorizontalAlignment)
    , mScreenSpaceSize(0.0f, 0.0f)
{
}

HudTextField::HudTextField(
    const std::string& fontName,
    const float fontSize,
    const glm::vec3& color,
    const float lineMaxSize,
    const int32_t numberOfLines,
    const eTextHorizontalAlignmentType textHorizontalAlignment)
    : mTextFieldId(UniqueFontTextIdGenerator::GenerateUniqueFontTextId())
    , mIsVisible(false)
    , mFontName(fontName)
    , mFontSize(fontSize)
    , mText()
    , mColor(color)
    , mPosition()
    , mLineMaxWidth(lineMaxSize)
    , mNumberOfLines(numberOfLines)
    , mTextHorizontalAlignment(textHorizontalAlignment)
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

float HudTextField::GetFontSize() const
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

float HudTextField::GetLineMaxSize() const
{
    return mLineMaxWidth;
}

int32_t HudTextField::GetNumberOfLines() const
{
    return mNumberOfLines;
}

eTextHorizontalAlignmentType HudTextField::GetTextHorizontalAlignment() const
{
    return mTextHorizontalAlignment;
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

void HudTextField::SetTextScreenSpaceSize(const glm::vec2& screenSpaceSize)
{
    mScreenSpaceSize = screenSpaceSize;
}

glm::vec2 HudTextField::GetScreenSpaceSize() const
{
    return mScreenSpaceSize;
}
} // namespace EngineCore