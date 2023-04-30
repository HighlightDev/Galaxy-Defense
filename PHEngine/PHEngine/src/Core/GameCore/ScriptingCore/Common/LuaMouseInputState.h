#pragma once

namespace EngineCore
{
   namespace Scripts 
   {
      enum class eLuaMouseInputPressState
      {
         MOUSE_BUTTON_RELEASED = 0,
         MOUSE_BUTTON_PRESSED = 1
      };

      enum class eLuaMouseInputCursorHoverState
      {
         CURSOR_HOVER_LEAVED = 0,
         CURSOR_HOVER_ENTERED = 1
      };
   }
}