#include "RayCollisionEvent.h"

namespace Event
{
    template class TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::weak_ptr<MissileActor>, std::weak_ptr<Actor>>>;
}