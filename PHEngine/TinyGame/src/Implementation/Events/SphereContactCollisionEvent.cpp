#include "SphereContactCollisionEvent.h"

using namespace Event;

namespace Event
{
    template class TEvent<MultipleDataEventPolicy<uint64_t, std::vector<uint64_t>>>;
}