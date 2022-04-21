#include "KeyboardBindings.h"
#include "Core/CommonCore/Assertion.h"

#include <algorithm>

using namespace Event;

namespace EngineCore
{

   Keys DefaultKeyboardBindings::GetMappedWithActionKey(eKeyActionType actionType)
   {
      Keys result = Keys::None;

      switch (actionType)
      {
      case EngineCore::eKeyActionType::ACTION_MOVE_FORWARD:
         result = Keys::W;
         break;
      case EngineCore::eKeyActionType::ACTION_MOVE_LEFT:
         result = Keys::A;
         break;
      case EngineCore::eKeyActionType::ACTION_MOVE_RIGHT:
         result = Keys::D;
         break;
      case EngineCore::eKeyActionType::ACTION_MOVE_BACK:
         result = Keys::S;
         break;
      case EngineCore::eKeyActionType::ACTION_JUMP:
         result = Keys::Space;
         break;
      }

      return result;
   }

   eKeyActionType DefaultKeyboardBindings::GetMappedWithKeyAction(Keys key)
   {
      eKeyActionType result = eKeyActionType::NONE;

      switch (key)
      {
      case Keys::W:
         result = eKeyActionType::ACTION_MOVE_FORWARD;
         break;
      case Keys::A:
         result = eKeyActionType::ACTION_MOVE_LEFT;
         break;
      case Keys::D:
         result = eKeyActionType::ACTION_MOVE_RIGHT;
         break;
      case Keys::S:
         result = eKeyActionType::ACTION_MOVE_BACK;
         break;
      case Keys::Space:
         result = eKeyActionType::ACTION_JUMP;
         break;
      }

      return result;
   }

   KeyboardBindings::KeyboardBindings(std::shared_ptr<IActionBinding> actionBindings)
       : KeyboardButtonDownEvent(), mActionBindings(actionBindings), mKeyboardMaskVec(), mReleasedKeysOnCurrentTick(), mPressedKeysOnCurrentTick()
   {
      KeyboardButtonDownEvent::GetInstance()->AddListener(this);

      mReleasedKeysOnCurrentTick.reserve(15); // 15 should be enough for beginning
      mPressedKeysOnCurrentTick.reserve(15);
   }

   KeyboardBindings::~KeyboardBindings()
   {
      KeyboardButtonDownEvent::GetInstance()->RemoveListener(this);
   }

   void KeyboardBindings::ProcessEvent(const KeyboardButtonDownEvent::EventData_t &eventData)
   {
      mKeyboardMaskVec = std::get<0>(eventData);

      UpdateKyboardState();
   }

   void KeyboardBindings::UpdateKyboardState()
   {
      mPressedKeysOnCurrentTick.clear();
      std::for_each(mKeyboardMaskVec.begin(), mKeyboardMaskVec.end(),
                    [this](const auto &keyData)
                    { if (KeyState::PRESSED == keyData.State) {
                             mPressedKeysOnCurrentTick.emplace_back(keyData.Key);
                          } });

      mReleasedKeysOnCurrentTick.clear();
      std::for_each(mKeyboardMaskVec.begin(), mKeyboardMaskVec.end(),
                    [this](const auto &keyData)
                    { if (KeyState::RELEASED == keyData.State) {
                             mReleasedKeysOnCurrentTick.emplace_back(keyData.Key);
                          } });
   }

   bool KeyboardBindings::HasPressedKeys() const
   {
      return mPressedKeysOnCurrentTick.size() > 0;
   }

   KeyState KeyboardBindings::GetKeyState(eKeyActionType actionType) const
   {
      KeyState state = KeyState::RELEASED;

      const Keys key = mActionBindings->GetMappedWithActionKey(actionType);
      auto it = std::find_if(mKeyboardMaskVec.begin(),
                             mKeyboardMaskVec.end(), [=](const auto &keyData) -> bool
                             { return keyData.Key == key; });

      if (it != mKeyboardMaskVec.end())
      {
         state = it->State;
      }

      return state;
   }

   std::shared_ptr<IActionBinding> KeyboardBindings::GetActionBindings() const
   {
      return mActionBindings;
   }

   const std::vector<Keys> &KeyboardBindings::GetReleasedKeysOnCurrentTick() const
   {
      return mReleasedKeysOnCurrentTick;
   }

   const std::vector<Keys> &KeyboardBindings::GetPressedKeysOnCurrentTick() const
   {
      return mPressedKeysOnCurrentTick;
   }
}