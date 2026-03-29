#pragma once

#include <string>

namespace Game {
enum class eGameObjectsType { UNDEFINED, SPACESHIP, MISSILE, ENEMY_MISSILE, NEUTRAL_SPACE_OBJECT, SPACE_STATION, BARRIER };

std::string ToString(const eGameObjectsType gameObjectType);
} // namespace Game
