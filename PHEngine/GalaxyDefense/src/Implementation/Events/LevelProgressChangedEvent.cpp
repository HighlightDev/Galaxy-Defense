#include "LevelProgressChangedEvent.h"

namespace Event
{
    template class TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<eLevelProgressStatusType, std::string /*args*/>>;

    template class TEvent<eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<eLevelProgressStatusType, std::string /*args*/>>;
}