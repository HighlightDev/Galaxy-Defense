#include "MouseMovedEvent.h"

namespace Event
{
   template class TEvent<MouseMovedGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<glm::ivec4 /*X, Y, deltaX, deltaY*/>>;
   template class TEvent<MouseMovedLuaThreadEvent, eEventThreadType::LUA_THREAD, SingleDataEventPolicy<glm::ivec4 /*X, Y, deltaX, deltaY*/>>;
}
