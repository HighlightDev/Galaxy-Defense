#include "KinematicBodyMovedEvent.h"
namespace Event {
template class TEvent<
    KinematicBodyMovedGameThreadEvent,
    eEventThreadType::GAME_THREAD,
    SingleDataEventPolicy<std::weak_ptr<EnginePhysics::PhysicsDescriptor>, EngineCore::EulerAnglesTransform>>;
}
