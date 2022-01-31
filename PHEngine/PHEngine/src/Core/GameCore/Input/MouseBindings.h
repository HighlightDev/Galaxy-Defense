#pragma once

#include "Core/GameCore/Event/MouseMovedEvent.h"

#include <vector>
#include <glm/vec4.hpp>

using namespace Event;

namespace Game
{
   class MouseBindings 
      : public MouseMovedEvent
   {

      glm::ivec4 mLastMouseMoveEvent;

      bool bMouseMoveEventDirty;

   public:

      MouseBindings();

      virtual ~MouseBindings();

      virtual void ProcessEvent(const typename MouseMovedEvent::EventData_t& data) override;

      bool IsMouseMoveEventDirty() const;

      glm::ivec4 FlushMouseMoveEvent();

      protected:

      void PushMouseMoveEvent(const glm::ivec4& moveEvent);
   };

};

