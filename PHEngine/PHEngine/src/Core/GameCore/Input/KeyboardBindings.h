#pragma once

#include "Keys.h"
#include "KeyboardData.h"
#include "Core/GameCore/Event/KeyboardInputEvent.h"

#include <unordered_map>
#include <vector>
#include <memory>

using namespace Event;

namespace Game
{
   enum class eKeyActionType
   {
      NONE,
      ACTION_MOVE_FORWARD,
      ACTION_MOVE_LEFT,
      ACTION_MOVE_RIGHT,
      ACTION_MOVE_BACK,
      ACTION_JUMP
   };
   
   struct IActionBinding
   {
      virtual Keys GetMappedWithActionKey(eKeyActionType actionType) = 0;
      virtual eKeyActionType GetMappedWithKeyAction(Keys key) = 0;
   };

   struct DefaultKeyboardBindings : public IActionBinding
   {
      virtual Keys GetMappedWithActionKey(eKeyActionType actionType) override;

      virtual eKeyActionType GetMappedWithKeyAction(Keys key) override;
   };

   class KeyboardBindings 
      : public KeyboardButtonDownEvent
   {
      std::shared_ptr<IActionBinding> mActionBindings;

      std::vector<KeyboardData> mKeyboardMaskVec;

      std::vector<Keys> mReleasedKeysOnCurrentTick;

      std::vector<Keys> mPressedKeysOnCurrentTick;

      size_t mPressedKeysCount = 0;

   public:

      KeyboardBindings(std::shared_ptr<IActionBinding> actionBindings);

      ~KeyboardBindings();

      virtual void ProcessEvent(const KeyboardButtonDownEvent::EventData_t& data) override;

      bool HasPressedKeys() const;

      void KeyPress(Keys key);

      void KeyRelease(Keys key);

      KeyState GetKeyState(eKeyActionType actionType) const;

      std::shared_ptr<IActionBinding> GetActionBindings() const;

      std::vector<Keys> GetReleasedKeysOnCurrentTickAndInvalidateVector();

      std::vector<Keys> GetPressedKeysOnCurrentTickAndInvalidateVector();
   };

};

