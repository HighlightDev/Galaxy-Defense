#include "MouseScrollEvent.h"

namespace Event {
template class TEvent<
    MouseScrollRootEvent,
    eEventThreadType::GAME_THREAD,
    SingleDataEventPolicy<glm::ivec2, EngineCore::eMouseScrollDirection, float>>;

template class TEvent<
    MouseScrollGameThreadEvent,
    eEventThreadType::GAME_THREAD,
    SingleDataEventPolicy<EngineCore::eMouseEventTargetReceiverType, EngineCore::eMouseScrollDirection, float>>;

template class TEvent<
    MouseScrollLuaThreadEvent,
    eEventThreadType::LUA_THREAD,
    SingleDataEventPolicy<EngineCore::eMouseScrollDirection, float>>;
} // namespace Event
