#include "RayCollisionEvent.h"

using namespace Event;

namespace Event
{
    template class TEvent<MultipleDataEventPolicy<std::weak_ptr<MissileActor>, std::weak_ptr<Actor>>>;
}