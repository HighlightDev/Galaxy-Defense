#include "MouseButtonDownEvent.h"

namespace Event {
template class TEvent<
    MouseButtonDownRootEvent,
    eEventThreadType::GAME_THREAD,
    SingleDataEventPolicy<glm::ivec2, std::vector<MouseKeysData>>>;

template class TEvent<
    MouseButtonDownGameThreadEvent,
    eEventThreadType::GAME_THREAD,
    SingleDataEventPolicy<eMouseEventTargetReceiverType, std::vector<MouseKeysData>>>;

template class TEvent<
    MouseButtonDownLuaThreadEvent,
    eEventThreadType::LUA_THREAD,
    SingleDataEventPolicy<std::vector<MouseKeysData>>>;
} // namespace Event
