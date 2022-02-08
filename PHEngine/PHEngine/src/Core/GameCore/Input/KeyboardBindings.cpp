#include "KeyboardBindings.h"
#include "Core/CommonCore/Assertion.h"

#include <algorithm>

using namespace Event;

namespace Game
{

   Keys DefaultKeyboardBindings::GetMappedWithActionKey(eKeyActionType actionType)
   {
      Keys result = Keys::None;

      switch (actionType)
      {
      case Game::eKeyActionType::ACTION_MOVE_FORWARD:
         result = Keys::W;
         break;
      case Game::eKeyActionType::ACTION_MOVE_LEFT:
         result = Keys::A;
         break;
      case Game::eKeyActionType::ACTION_MOVE_RIGHT:
         result = Keys::D;
         break;
      case Game::eKeyActionType::ACTION_MOVE_BACK:
         result = Keys::S;
         break;
      case Game::eKeyActionType::ACTION_JUMP:
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
       : KeyboardButtonDownEvent(), mActionBindings(actionBindings), mReleasedKeysOnCurrentTick(), mPressedKeysOnCurrentTick()
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
      const auto &data = std::get<0>(eventData);

      if (data.State == KeyState::PRESSED)
      {
         KeyPress(data.Key);
      }
      else
      {
         KeyRelease(data.Key);
      }
   }

   bool KeyboardBindings::HasPressedKeys() const
   {
      return mPressedKeysCount > 0;
   }

   bool KeyboardBindings::HasPressedSpecificKey(const Keys key) const
   {
      return std::find(mPressedKeysOnCurrentTick.begin(), mPressedKeysOnCurrentTick.end(),
                       key) != mPressedKeysOnCurrentTick.end();
   }

   void KeyboardBindings::KeyPress(Keys key)
   {
      auto it = std::find_if(mKeyboardMaskVec.begin(),
                             mKeyboardMaskVec.end(), [=](const auto &keyData) -> bool
                             { return keyData.Key == key; });

      if (it == mKeyboardMaskVec.end())
      {
         mKeyboardMaskVec.emplace_back(key, KeyState::PRESSED);
         mPressedKeysOnCurrentTick.push_back(key);
      }
      else
      {
         if (it->State == KeyState::RELEASED)
         {
            mPressedKeysOnCurrentTick.push_back(key);
         }

         it->State = KeyState::PRESSED;
      }

      mPressedKeysCount++;
   }

   void KeyboardBindings::KeyRelease(Keys key)
   {
      auto it = std::find_if(mKeyboardMaskVec.begin(),
                             mKeyboardMaskVec.end(), [=](const auto &keyData) -> bool
                             { return keyData.Key == key; });

      if (it != mKeyboardMaskVec.end())
      {
         if (it->State == KeyState::PRESSED)
         {
            assert(std::find(mReleasedKeysOnCurrentTick.begin(), mReleasedKeysOnCurrentTick.end(), key) == mReleasedKeysOnCurrentTick.end());
            mReleasedKeysOnCurrentTick.push_back(key);
         }

         it->State = KeyState::RELEASED;
         mPressedKeysCount--;
      }
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

   std::vector<Keys> KeyboardBindings::GetReleasedKeysOnCurrentTickAndInvalidateVector()
   {
      std::vector<Keys> result;
      std::swap(result, mReleasedKeysOnCurrentTick);
      return result;
   }

   std::vector<Keys> KeyboardBindings::GetPressedKeysOnCurrentTickAndInvalidateVector()
   {
      std::vector<Keys> result;
      std::swap(result, mPressedKeysOnCurrentTick);
      return result;
   }
}