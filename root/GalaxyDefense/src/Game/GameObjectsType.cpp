#include "GameObjectsType.h"

#include "Core/CommonCore/Assertion.h"

namespace Game {
std::string ToString(const eGameObjectsType gameObjectType)
{
    switch (gameObjectType) {
    case eGameObjectsType::UNDEFINED:
        return "UNDEFINED";
    case eGameObjectsType::SPACESHIP:
        return "SPACESHIP";
    case eGameObjectsType::TOWER_MISSILE:
        return "TOWER_MISSILE";
    case eGameObjectsType::SPACESHIP_MISSILE:
        return "SPACESHIP_MISSILE";
    case eGameObjectsType::NEUTRAL_SPACE_OBJECT:
        return "NEUTRAL_SPACE_OBJECT";
    case eGameObjectsType::SPACE_STATION:
        return "SPACE_STATION";
    case eGameObjectsType::BARRIER:
        return "BARRIER";
    case eGameObjectsType::LOOT:
        return "LOOT";
    default:
        ext_assert(false, "Unknown game object type");
        return "UNKNOWN";
    }
}
} // namespace Game
