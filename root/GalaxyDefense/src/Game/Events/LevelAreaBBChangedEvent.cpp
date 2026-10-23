#include "LevelAreaBBChangedEvent.h"

namespace Event {
template class TEvent<
    LevelAreaBBChangedGameThreadEvent,
    eEventThreadType::GAME_THREAD,
    MultipleDataEventPolicy<BoundingBox2D<glm::vec2>>>;

template class TEvent<
    LevelAreaBBChangedLuaThreadEvent,
    eEventThreadType::LUA_THREAD,
    MultipleDataEventPolicy<BoundingBox2D<glm::vec2>>>;
} // namespace Event
