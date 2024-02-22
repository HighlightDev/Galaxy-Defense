#include "HudTextField.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Event/TextEvent.h"
#include "Core/GameCore/GUI/Common/UniqueFontTextIdGenerator.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace Event;
using namespace TinyLogger;

namespace EngineCore
{
  HudTextField::HudTextField(const std::string &fontName,
                       const float fontSize,
                       const std::string &text,
                       const glm::vec3 &color,
                       const glm::vec2 &position,
                       const float lineMaxSize,
                       const int32_t numberOfLines,
                       const eTextHorizontalAlignmentType textHorizontalAlignment)
      : mTextFieldId(-1),
        mIsVisible(true),
        mFontName(fontName),
        mFontSize(fontSize),
        mText(text),
        mColor(color),
        mPosition(position),
        mLineMaxWidth(lineMaxSize),
        mNumberOfLines(numberOfLines),
        mTextHorizontalAlignment(textHorizontalAlignment),
        mIsRegistered(false),
        mScreenSpaceSize(0.0f, 0.0f)
  {
  }

  HudTextField::HudTextField(const std::string &fontName,
                       const float fontSize,
                       const glm::vec3 &color,
                       const float lineMaxSize,
                       const int32_t numberOfLines,
                       const eTextHorizontalAlignmentType textHorizontalAlignment)
      : mTextFieldId(-1),
        mIsVisible(false),
        mFontName(fontName),
        mFontSize(fontSize),
        mText(),
        mColor(color),
        mPosition(),
        mLineMaxWidth(lineMaxSize),
        mNumberOfLines(numberOfLines),
        mTextHorizontalAlignment(textHorizontalAlignment),
        mIsRegistered(false)
  {
  }

  void HudTextField::RegisterText(const bool receiveUpdateOnTextScreenSpaceSizeChanged)
  {
    assert(!mIsRegistered);
    mTextFieldId = UniqueFontTextIdGenerator::GenerateUniqueFontTextId();
    mIsRegistered = true;
    TextRegisterGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, shared_from_this(), eRegisterType::REGISTER, receiveUpdateOnTextScreenSpaceSizeChanged);

    LogInfo("HudTextField::RegisterText => Registered text with id = ", mTextFieldId);
  }

  void HudTextField::UnregisterText()
  {
    assert(mIsRegistered);
    mIsRegistered = false;
    TextRegisterGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, shared_from_this(), eRegisterType::UNREGISTER, false);

    LogInfo( "HudTextField::UnregisterText => Unregistered text with id = ", mTextFieldId);
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

  bool HudTextField::GetIsRegistered() const
  {
    return mIsRegistered;
  }

  void HudTextField::SetText(const std::string &text)
  {
    if (mText != text)
    {
      mText = text;

      if (mIsRegistered)
      {
        TextDataChangedGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION,
                                                       shared_from_this(),
                                                       eTextChangedDataType::TEXT);
      }
    }
  }

  void HudTextField::SetVisibility(const bool isVisible)
  {
    if (mIsVisible != isVisible)
    {
      mIsVisible = isVisible;

      if (mIsRegistered)
      {
        TextDataChangedGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION,
                                                       shared_from_this(),
                                                       eTextChangedDataType::VISIBILITY);
      }
    }
  }

  void HudTextField::SetColor(const glm::vec3 &color)
  {
    mColor = color;

    if (mIsRegistered)
    {
      TextDataChangedGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION,
                                                     shared_from_this(),
                                                     eTextChangedDataType::COLOR);
    }
  }

  void HudTextField::SetPosition(const glm::vec2 &position)
  {
    mPosition = position;

    if (mIsRegistered)
    {
      TextDataChangedGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION,
                                                     shared_from_this(),
                                                     eTextChangedDataType::OFFSET);
    }
  }

  void HudTextField::SetTextScreenSpaceSize(const glm::vec2& screenSpaceSize)
  {
    mScreenSpaceSize = screenSpaceSize;
  }

  glm::vec2 HudTextField::GetScreenSpaceSize() const
  {
    return mScreenSpaceSize;
  }
}