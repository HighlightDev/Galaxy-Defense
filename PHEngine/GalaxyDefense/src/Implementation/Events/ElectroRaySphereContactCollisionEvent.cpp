#include "ElectroRaySphereContactCollisionEvent.h"

namespace Event
{
    template class TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<int32_t, std::vector<int32_t>>>;
}