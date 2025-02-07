#include "PlayerMovedEvent.h"

namespace Event {
template class TEvent<
    PlayerMovedGameThreadEvent,
    eEventThreadType::GAME_THREAD,
    SingleDataEventPolicy<std::weak_ptr<EngineCore::Transform>>>;
}