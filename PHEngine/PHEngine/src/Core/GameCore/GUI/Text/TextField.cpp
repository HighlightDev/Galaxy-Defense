#include "TextField.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Event/TextEvent.h"

#include <TinyLogger/LogInterface.h>

using namespace Event;
using namespace TinyLogger;

namespace EngineCore
{
  size_t TextField::s_TotalTextFieldId = 0;

  TextField::TextField(const std::string &fontName,
                       const float fontSize,
                       const glm::vec3 &color,
                       const glm::vec2 &position,
                       const float lineMaxSize,
                       const int32_t numberOfLines,
                       const bool isCenteredText)
      : mTextFieldId(-1),
        mFontName(fontName),
        mFontSize(fontSize),
        mColor(color),
        mPosition(position),
        mLineMaxSize(lineMaxSize),
        mNumberOfLines(numberOfLines),
        mIsCenteredText(isCenteredText),
        mIsRegistered(false)
  {
  }

  void TextField::RegisterText()
  {
    assert(!mIsRegistered);
    mTextFieldId = s_TotalTextFieldId++;
    mIsRegistered = true;
    TextRegisterEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, this, eRegisterType::REGISTER);

    Logger::Out("TextField::RegisterText => Registered text with id = ", mTextFieldId);
  }

  void TextField::UnregisterText()
  {
    assert(mIsRegistered);
    mIsRegistered = false;
    TextRegisterEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, this, eRegisterType::UNREGISTER);

    Logger::Out("TextField::UnregisterText => Unregistered text with id = ", mTextFieldId);
  }

  int32_t TextField::GetTextFieldId() const
  {
    return mTextFieldId;
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
}