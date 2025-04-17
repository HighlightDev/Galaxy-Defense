#include "MouseScrollEvent.h"

namespace Event {
template class TEvent<
    MouseScrollGameThreadEvent,
    eEventThreadType::GAME_THREAD,
    SingleDataEventPolicy<EngineCore::eMouseScrollDirection, float>>;

template class TEvent<
    MouseScrollLuaThreadEvent,
    eEventThreadType::LUA_THREAD,
    SingleDataEventPolicy<EngineCore::eMouseScrollDirection, float>>;
} // namespace Event
