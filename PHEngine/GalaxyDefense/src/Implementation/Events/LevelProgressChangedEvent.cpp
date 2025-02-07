#include "LevelProgressChangedEvent.h"

namespace Event {
template class TEvent<
    LevelProgressChangedEvent,
    eEventThreadType::GAME_THREAD,
    MultipleDataEventPolicy<eLevelProgressStatusType, std::string /*args*/>>;

template class TEvent<
    LuaLevelProgressChangedEvent,
    eEventThreadType::LUA_THREAD,
    MultipleDataEventPolicy<eLevelProgressStatusType, std::string /*args*/>>;
} // namespace Event