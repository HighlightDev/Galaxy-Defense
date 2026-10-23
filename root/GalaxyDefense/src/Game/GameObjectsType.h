#pragma once

#include <string>

namespace Game {
enum class eGameObjectsType {
    UNDEFINED,
    SPACESHIP,
    TOWER_MISSILE,
    SPACESHIP_MISSILE,
    NEUTRAL_SPACE_OBJECT,
    SPACE_STATION,
    BARRIER,
    LOOT
};

std::string ToString(const eGameObjectsType gameObjectType);
} // namespace Game
