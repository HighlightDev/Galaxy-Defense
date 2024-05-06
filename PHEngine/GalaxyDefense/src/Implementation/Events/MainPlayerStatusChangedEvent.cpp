#include "MainPlayerStatusChangedEvent.h"

namespace Event
{
    template class TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<eMainPlayerStatusType, std::string/*args*/>>;

    template class TEvent<eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<eMainPlayerStatusType, std::string/*args*/>>;
}