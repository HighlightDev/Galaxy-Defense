#include "ElectroRaySphereContactCollisionEvent.h"

namespace Event
{
    template class TEvent<ElectroRaySphereContactCollisionEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<int32_t, std::vector<int32_t>>>;
}