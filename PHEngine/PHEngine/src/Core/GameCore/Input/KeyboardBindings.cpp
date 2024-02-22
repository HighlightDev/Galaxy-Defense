#include "KeyboardBindings.h"
#include "Core/CommonCore/Assertion.h"

#include <algorithm>

using namespace Event;

namespace EngineCore
{

   eKeyboardKeys DefaultKeyboardBindings::GetMappedWithActionKey(eKeyActionType actionType)
   {
      eKeyboardKeys result = eKeyboardKeys::None;

      switch (actionType)
      {
      case EngineCore::eKeyActionType::ACTION_MOVE_FORWARD:
         result = eKeyboardKeys::W;
         break;
      case EngineCore::eKeyActionType::ACTION_MOVE_LEFT:
         result = eKeyboardKeys::A;
         break;
      case EngineCore::eKeyActionType::ACTION_MOVE_RIGHT:
         result = eKeyboardKeys::D;
         break;
      case EngineCore::eKeyActionType::ACTION_MOVE_BACK:
         result = eKeyboardKeys::S;
         break;
      case EngineCore::eKeyActionType::ACTION_JUMP:
         result = eKeyboardKeys::Space;
         break;
      }

      return result;
   }

   eKeyActionType DefaultKeyboardBindings::GetMappedWithKeyAction(eKeyboardKeys key)
   {
      eKeyActionType result = eKeyActionType::NONE;

      switch (key)
      {
      case eKeyboardKeys::W:
         result = eKeyActionType::ACTION_MOVE_FORWARD;
         break;
      case eKeyboardKeys::A:
         result = eKeyActionType::ACTION_MOVE_LEFT;
         break;
      case eKeyboardKeys::D:
         result = eKeyActionType::ACTION_MOVE_RIGHT;
         break;
      case eKeyboardKeys::S:
         result = eKeyActionType::ACTION_MOVE_BACK;
         break;
      case eKeyboardKeys::Space:
         result = eKeyActionType::ACTION_JUMP;
         break;
      }

      return result;
   }

   KeyboardBindings::KeyboardBindings(std::shared_ptr<IActionBinding> actionBindings)
       : KeyboardButtonDownGameThreadEvent(), mActionBindings(actionBindings), mKeyboardMaskVec(), mReleasedKeysOnCurrentTick(), mPressedKeysOnCurrentTick()
   {
      mReleasedKeysOnCurrentTick.reserve(15); // 15 should be enough for beginning
      mPressedKeysOnCurrentTick.reserve(15);
   }

   KeyboardBindings::~KeyboardBindings()
   {
      KeyboardButtonDownGameThreadEvent::GetInstance()->RemoveListener(KeyboardButtonDownGameThreadEvent::GetInstanceId());
   }

   void KeyboardBindings::Initialize()
   {
      KeyboardButtonDownGameThreadEvent::GetInstance()->AddListener(shared_from_this());
   }

   void KeyboardBindings::ProcessEvent(const KeyboardButtonDownGameThreadEvent::EventData_t &eventData)
   {
      mKeyboardMaskVec = std::get<0>(eventData);

      UpdateKyboardState();
   }

   void KeyboardBindings::UpdateKyboardState()
   {
      mPressedKeysOnCurrentTick.clear();
      mReleasedKeysOnCurrentTick.clear();

      for (const auto &keyboardKeyData : mKeyboardMaskVec)
      {
         if (KeyState::PRESSED == keyboardKeyData.State)
         {
            mPressedKeysOnCurrentTick.emplace_back(keyboardKeyData.Key);
         }
         else
         {
            mReleasedKeysOnCurrentTick.emplace_back(keyboardKeyData.Key);
         }
      }
   }

   bool KeyboardBindings::HasPressedKeys() const
   {
      return mPressedKeysOnCurrentTick.size() > 0;
   }

   bool KeyboardBindings::HasReleasedKeys() const
   {
      return mReleasedKeysOnCurrentTick.size() > 0;
   }

   KeyState KeyboardBindings::GetKeyStateByActionType(eKeyActionType actionType) const
   {
      KeyState state = KeyState::RELEASED;

      const eKeyboardKeys key = mActionBindings->GetMappedWithActionKey(actionType);
      auto it = std::find_if(mKeyboardMaskVec.begin(),
                             mKeyboardMaskVec.end(), [=](const auto &keyData) -> bool
                             { return keyData.Key == key; });

      if (it != mKeyboardMaskVec.end())
      {
         state = it->State;
      }

      return state;
   }

   KeyState KeyboardBindings::GetStateByKey(const eKeyboardKeys key) const
   {
      KeyState state = KeyState::RELEASED;

      auto it = std::find_if(mKeyboardMaskVec.begin(),
                             mKeyboardMaskVec.end(), [=](const auto &keyData) -> bool
                             { return keyData.Key == key; });

      if (it != mKeyboardMaskVec.end())
      {
         state = it->State;
      }

      return state;
   }

   void KeyboardBindings::ClearKeyboardCache()
   {
      mKeyboardMaskVec.clear();
      mReleasedKeysOnCurrentTick.clear();
      mPressedKeysOnCurrentTick.clear();
   }

   std::shared_ptr<IActionBinding> KeyboardBindings::GetActionBindings() const
   {
      return mActionBindings;
   }

   const std::vector<eKeyboardKeys> &KeyboardBindings::GetReleasedKeys() const
   {
      return mReleasedKeysOnCurrentTick;
   }

   const std::vector<eKeyboardKeys> &KeyboardBindings::GetPressedKeys() const
   {
      return mPressedKeysOnCurrentTick;
   }

   void KeyboardBindings::SetIsReceivingKeyboardEvents(const bool receiveKeyboardEvents)
   {
      if (bReceiveKeyboardEvents != receiveKeyboardEvents)
      {
         bReceiveKeyboardEvents = receiveKeyboardEvents;
         if (receiveKeyboardEvents)
         {
            SubscribeOnEvents();
         }
         else
         {
            UnsubscribeFromEvents();
            ClearKeyboardCache();
         }
      }
   }

   void KeyboardBindings::UnsubscribeFromEvents()
   {
      KeyboardButtonDownGameThreadEvent::GetInstance()->RemoveListener(KeyboardButtonDownGameThreadEvent::GetInstanceId());
   }

   void KeyboardBindings::SubscribeOnEvents()
   {
      KeyboardButtonDownGameThreadEvent::GetInstance()->AddListener(shared_from_this());
   }
}