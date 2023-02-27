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
      eKeyboardKeys GetMappedWithActionKey(eKeyActionType actionType) override;

      eKeyActionType GetMappedWithKeyAction(eKeyboardKeys key) override;
   };

   class KeyboardBindings
       : public KeyboardButtonDownEvent
   {
      std::shared_ptr<IActionBinding> mActionBindings;

      std::vector<KeyboardKeysData> mKeyboardMaskVec;

      std::vector<eKeyboardKeys> mReleasedKeysOnCurrentTick;

      std::vector<eKeyboardKeys> mPressedKeysOnCurrentTick;

      bool bReceiveKeyboardEvents{true};

   public:
      KeyboardBindings(std::shared_ptr<IActionBinding> actionBindings);

      ~KeyboardBindings();

      void ProcessEvent(const typename KeyboardButtonDownEvent::EventData_t &data) override;

      void UpdateKyboardState();

      bool HasPressedKeys() const;

      bool HasReleasedKeys() const;

      KeyState GetKeyStateByActionType(eKeyActionType actionType) const;

      KeyState GetStateByKey(const eKeyboardKeys key) const;

      std::shared_ptr<IActionBinding> GetActionBindings() const;

      const std::vector<eKeyboardKeys> &GetReleasedKeys() const;

      const std::vector<eKeyboardKeys> &GetPressedKeys() const;

      void SetIsReceivingKeyboardEvents(const bool receiveKeyboardEvents);

      void ClearKeyboardCache();

   private:
      void UnsubscribeFromEvents();

      void SubscribeOnEvents();
   };

};
