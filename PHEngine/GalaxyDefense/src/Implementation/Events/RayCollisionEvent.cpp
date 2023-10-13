#include "RayCollisionEvent.h"

using namespace Event;

namespace Event
{
    template class TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::weak_ptr<MissileActor>, std::weak_ptr<Actor>>>;
}