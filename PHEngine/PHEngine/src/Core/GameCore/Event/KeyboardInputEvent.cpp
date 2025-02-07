#include "KeyboardInputEvent.h"

namespace Event {
template class TEvent<
    KeyboardButtonDownGameThreadEvent,
    eEventThreadType::GAME_THREAD,
    SingleDataEventPolicy<std::vector<KeyboardKeysData>>>;

template class TEvent<
    KeyboardButtonDownLuaThreadEvent,
    eEventThreadType::LUA_THREAD,
    SingleDataEventPolicy<std::vector<KeyboardKeysData>>>;
} // namespace Event
