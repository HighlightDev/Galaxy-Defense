#include "BroadcastEvent.h"

namespace Event
{
    template class TEvent<BroadcastGameThreadEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::string /*event header*/, std::string /*json parameters*/>>;

    template class TEvent<BroadcastLuaThreadEvent, eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<std::string /*event header*/, std::string /*json parameters*/>>;
}