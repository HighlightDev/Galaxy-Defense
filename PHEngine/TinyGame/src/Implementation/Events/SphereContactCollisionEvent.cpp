#include "SphereContactCollisionEvent.h"

using namespace Event;

namespace Event
{
    template class TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<uint64_t, std::vector<uint64_t>>>;
}