#pragma once

namespace EngineCore
{
    enum class eMouseScrollDirection
    {
        Undefined,
        ZoomIn,
        ZoomOut,
    };

    enum class eMouseEventTargetReceiverType
   {
      SCENE_GAME_OBJECTS,
      UI_INPUT_SYSTEM
   };
}