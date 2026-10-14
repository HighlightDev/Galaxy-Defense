#include "ShootRayCollisionEvent.h"

namespace Event {
template class TEvent<
    ShootRayCollisionEvent,
    eEventThreadType::GAME_THREAD,
    MultipleDataEventPolicy<std::weak_ptr<MissileActor>, std::weak_ptr<Actor>, eRayType>>;
}