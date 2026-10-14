#include "MouseButtonDownEvent.h"

namespace Event {
template class TEvent<
    MouseButtonDownRootEvent,
    eEventThreadType::GAME_THREAD,
    MultipleDataEventPolicy<glm::ivec2, std::vector<MouseKeysData>>>;

template class TEvent<
    MouseButtonDownGameThreadEvent,
    eEventThreadType::GAME_THREAD,
    MultipleDataEventPolicy<eMouseEventTargetReceiverType, std::vector<MouseKeysData>>>;

template class TEvent<
    MouseButtonDownLuaThreadEvent,
    eEventThreadType::LUA_THREAD,
    MultipleDataEventPolicy<std::vector<MouseKeysData>>>;
} // namespace Event
