#include "ElectroRayCollisionEvent.h"

namespace Event {
template class TEvent<
    ElectroRayCollisionEvent,
    eEventThreadType::GAME_THREAD,
    MultipleDataEventPolicy<std::weak_ptr<MissileActor>, std::weak_ptr<Actor>>>;
}