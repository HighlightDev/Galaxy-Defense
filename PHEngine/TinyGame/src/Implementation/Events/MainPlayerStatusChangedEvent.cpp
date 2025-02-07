#include "MainPlayerStatusChangedEvent.h"

using namespace Event;

namespace Event {
template class TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<eMainPlayerStatusType>>;

template class TEvent<eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<eMainPlayerStatusType>>;
} // namespace Event