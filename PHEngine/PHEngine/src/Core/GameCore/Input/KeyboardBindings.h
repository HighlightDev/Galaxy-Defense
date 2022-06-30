#pragma once

#include "Keys.h"
#include "InputDeviceKeyData.h"
#include "Core/GameCore/Event/KeyboardInputEvent.h"

#include <unordered_map>
#include <vector>
#include <memory>

using namespace Event;

namespace EngineCore
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
      virtual eKeyboardKeys GetMappedWithActionKey(eKeyActionType actionType) = 0;
      virtual eKeyActionType GetMappedWithKeyAction(eKeyboardKeys key) = 0;
   };

   struct DefaultKeyboardBindings : public IActionBinding
   {
      virtual eKeyboardKeys GetMappedWithActionKey(eKeyActionType actionType) override;

      virtual eKeyActionType GetMappedWithKeyAction(eKeyboardKeys key) override;
   };

   class KeyboardBindings 
      : public KeyboardButtonDownEvent
   {
      std::shared_ptr<IActionBinding> mActionBindings;

      std::vector<KeyboardKeysData> mKeyboardMaskVec;

      std::vector<eKeyboardKeys> mReleasedKeysOnCurrentTick;

      std::vector<eKeyboardKeys> mPressedKeysOnCurrentTick;

   public:

      KeyboardBindings(std::shared_ptr<IActionBinding> actionBindings);

      ~KeyboardBindings();

      virtual void ProcessEvent(const typename KeyboardButtonDownEvent::EventData_t& data) override;

      void UpdateKyboardState();

      bool HasPressedKeys() const;

      KeyState GetKeyState(eKeyActionType actionType) const;

      std::shared_ptr<IActionBinding> GetActionBindings() const;

      const std::vector<eKeyboardKeys>& GetReleasedKeys() const;

      const std::vector<eKeyboardKeys>& GetPressedKeys() const;
   };

};

