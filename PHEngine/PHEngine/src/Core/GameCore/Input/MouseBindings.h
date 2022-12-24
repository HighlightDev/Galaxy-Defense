#pragma once

#include "Core/GameCore/Event/MouseMovedEvent.h"
#include "Core/GameCore/Event/MouseScrollEvent.h"
#include "Core/GameCore/Event/MouseButtonDownEvent.h"
#include "Core/GameCore/Input/InputDeviceKeyData.h"

#include <vector>
#include <glm/vec4.hpp>

using namespace Event;

namespace EngineCore
{
   class MouseBindings
       : public MouseMovedEvent,
         public MouseScrollEvent,
         public MouseButtonDownEvent
   {
      glm::ivec4 mLastMouseMoveEvent;

      bool bMouseMoveEventDirty;

      eMouseScrollDirection mLastMouseScrollDirectionEvent;

      bool bMouseScrollEventDirty;

      std::vector<MouseKeysData> mMouseKeysMaskVec;

      size_t mPressedMouseKeysCount;

      bool bReceiveMouseEvents{true};

   public:
      MouseBindings();

      virtual ~MouseBindings();

      virtual void ProcessEvent(const typename MouseMovedEvent::EventData_t &data) override;

      virtual void ProcessEvent(const typename MouseScrollEvent::EventData_t &data) override;

      virtual void ProcessEvent(const typename MouseButtonDownEvent::EventData_t &data) override;

      bool IsMouseMoveEventDirty() const;

      bool IsMouseScrollEventDirty() const;

      eMouseScrollDirection FlushMouseScrollEvent();

      glm::ivec4 FlushMouseMoveEvent();

      glm::ivec4 GetLastMouseCursorPosition() const;

      void ClearMouseScrollCache();

      void ClearMouseMoveCache();

      KeyState GetKeyState(const eMouseKeys mouseButtonKey) const;

      void SetIsReceivingMouseEvents(const bool receiveMouseEvents);

   protected:
      void PushMouseMoveEvent(const glm::ivec4 &moveEvent);

      void PushMouseScrollEvent(const eMouseScrollDirection mouseScrollEvent);

   private:
      void UnsubscribeFromEvents();

      void SubscribeOnEvents();
   };

};
