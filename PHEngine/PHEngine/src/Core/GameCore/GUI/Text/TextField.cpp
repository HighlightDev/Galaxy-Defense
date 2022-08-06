#include "TextField.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Event/TextEvent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/LoggerExtension.h"

#include <TinyLogger/LogInterface.h>

using namespace Event;
using namespace TinyLogger;

namespace EngineCore
{
  size_t TextField::s_TotalTextFieldId = 0;

  TextField::TextField(const std::string &fontName,
                       const float fontSize,
                       const std::string &text,
                       const glm::vec3 &color,
                       const glm::vec2 &position,
                       const float lineMaxSize,
                       const int32_t numberOfLines,
                       const bool isCenteredText)
      : mTextFieldId(-1),
        mIsVisible(true),
        mFontName(fontName),
        mFontSize(fontSize),
        mText(text),
        mColor(color),
        mPosition(position),
        mLineMaxSize(lineMaxSize),
        mNumberOfLines(numberOfLines),
        mIsCenteredText(isCenteredText),
        mIsRegistered(false),
        mScreenSpaceSize(0.0f, 0.0f)
  {
  }

  TextField::TextField(const std::string &fontName,
                       const float fontSize,
                       const glm::vec3 &color,
                       const float lineMaxSize,
                       const int32_t numberOfLines,
                       const bool isCenteredText)
      : mTextFieldId(-1),
        mIsVisible(false),
        mFontName(fontName),
        mFontSize(fontSize),
        mText(),
        mColor(color),
        mPosition(),
        mLineMaxSize(lineMaxSize),
        mNumberOfLines(numberOfLines),
        mIsCenteredText(isCenteredText),
        mIsRegistered(false)
  {
  }

  void TextField::RegisterText(const bool receiveUpdateOnTextScreenSpaceSizeChanged)
  {
    assert(!mIsRegistered);
    mTextFieldId = s_TotalTextFieldId++;
    mIsRegistered = true;
    TextRegisterEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, shared_from_this(), eRegisterType::REGISTER, receiveUpdateOnTextScreenSpaceSizeChanged);

    LogInfo( "TextField::RegisterText => Registered text with id = ", mTextFieldId);
  }

  void TextField::UnregisterText()
  {
    assert(mIsRegistered);
    mIsRegistered = false;
    TextRegisterEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, shared_from_this(), eRegisterType::UNREGISTER, false);

    LogInfo( "TextField::UnregisterText => Unregistered text with id = ", mTextFieldId);
  }

  std::string TextField::GetText() const
  {
    return mText;
  }

  int32_t TextField::GetTextFieldId() const
  {
    return mTextFieldId;
  }

  bool TextField::GetIsVisible() const
  {
    return mIsVisible;
  }

  std::string TextField::GetFontName() const
  {
    return mFontName;
  }

  float TextField::GetFontSize() const
  {
    return mFontSize;
  }

  glm::vec3 TextField::GetColor() const
  {
    return mColor;
  }

  glm::vec2 TextField::GetPosition() const
  {
    return mPosition;
  }

  float TextField::GetLineMaxSize() const
  {
    return mLineMaxSize;
  }

  int32_t TextField::GetNumberOfLines() const
  {
    return mNumberOfLines;
  }

  bool TextField::GetIsCentered() const
  {
    return mIsCenteredText;
  }

  bool TextField::GetIsRegistered() const
  {
    return mIsRegistered;
  }

  void TextField::SetText(const std::string &text)
  {
    if (mText != text)
    {
      mText = text;

      if (mIsRegistered)
      {
        TextDataChangedEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION,
                                                       shared_from_this(),
                                                       eTextChangedDataType::TEXT);
      }
    }
  }

  void TextField::SetVisibility(const bool isVisible)
  {
    if (mIsVisible != isVisible)
    {
      mIsVisible = isVisible;

      if (mIsRegistered)
      {
        TextDataChangedEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION,
                                                       shared_from_this(),
                                                       eTextChangedDataType::VISIBILITY);
      }
    }
  }

  void TextField::SetColor(const glm::vec3 &color)
  {
    mColor = color;

    if (mIsRegistered)
    {
      TextDataChangedEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION,
                                                     shared_from_this(),
                                                     eTextChangedDataType::COLOR);
    }
  }

  void TextField::SetPosition(const glm::vec2 &position)
  {
    mPosition = position;

    if (mIsRegistered)
    {
      TextDataChangedEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION,
                                                     shared_from_this(),
                                                     eTextChangedDataType::OFFSET);
    }
  }

  void TextField::SetTextScreenSpaceSize(const glm::vec2& screenSpaceSize)
  {
    mScreenSpaceSize = screenSpaceSize;
  }

  glm::vec2 TextField::GetScreenSpaceSize() const
  {
    return mScreenSpaceSize;
  }
}