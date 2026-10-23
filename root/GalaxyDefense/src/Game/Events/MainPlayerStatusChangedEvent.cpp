#include "MainPlayerStatusChangedEvent.h"

namespace Event {
template class TEvent<MainPlayerStatusChangedEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::string /*args*/>>;

template class TEvent<
    LuaMainPlayerStatusChangedEvent,
    eEventThreadType::LUA_THREAD,
    MultipleDataEventPolicy<std::string /*args*/>>;
} // namespace Event