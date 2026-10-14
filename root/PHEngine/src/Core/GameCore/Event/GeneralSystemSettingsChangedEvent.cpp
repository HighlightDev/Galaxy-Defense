#include "GeneralSystemSettingsChangedEvent.h"

namespace Event {
template class TEvent<
    GeneralSystemSettingsChangedGameThreadEvent,
    eEventThreadType::GAME_THREAD,
    MultipleDataEventPolicy<eSystemSettingsEventType /*event type*/, std::string /*json parameters*/>>;

template class TEvent<
    GeneralSystemSettingsChangedLuaThreadEvent,
    eEventThreadType::LUA_THREAD,
    MultipleDataEventPolicy<eSystemSettingsEventType /*event type*/, std::string /*json parameters*/>>;
} // namespace Event