#include "MouseScrollEvent.h"

namespace Event {
template class TEvent<
    MouseScrollGameThreadEvent,
    eEventThreadType::GAME_THREAD,
    SingleDataEventPolicy<EngineCore::eMouseScrollDirection>>;

template class TEvent<
    MouseScrollLuaThreadEvent,
    eEventThreadType::LUA_THREAD,
    SingleDataEventPolicy<EngineCore::eMouseScrollDirection>>;
} // namespace Event
