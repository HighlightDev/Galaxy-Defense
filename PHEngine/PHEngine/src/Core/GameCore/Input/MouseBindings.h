#pragma once

#include "Core/GameCore/Event/MouseMovedEvent.h"
#include "Core/GameCore/Event/MouseScrollEvent.h"

#include <vector>
#include <glm/vec4.hpp>

using namespace Event;

namespace EngineCore
{
   class MouseBindings 
      : public MouseMovedEvent
      , public MouseScrollEvent
   {
      glm::ivec4 mLastMouseMoveEvent;

      bool bMouseMoveEventDirty;

      eMouseScrollDirection mLastMouseScrollDirectionEvent;

      bool bMouseScrollEventDirty;
   public:

      MouseBindings();

      virtual ~MouseBindings();

      virtual void ProcessEvent(const typename MouseMovedEvent::EventData_t& data) override;

      virtual void ProcessEvent(const typename MouseScrollEvent::EventData_t& data) override;

      bool IsMouseMoveEventDirty() const;

      bool IsMouseScrollEventDirty() const;

      eMouseScrollDirection FlushMouseScrollEvent();

      glm::ivec4 FlushMouseMoveEvent();

      protected:

      void PushMouseMoveEvent(const glm::ivec4& moveEvent);

      void PushMouseScrollEvent(const eMouseScrollDirection mouseScrollEvent);
   };

};

