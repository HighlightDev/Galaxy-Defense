#include "LevelFilesListChangedEvent.h"

namespace Event {
template class TEvent<
    LuaLevelFilesListChangedEvent,
    eEventThreadType::LUA_THREAD,
    SingleDataEventPolicy<std::vector<std::string>>>;
}
