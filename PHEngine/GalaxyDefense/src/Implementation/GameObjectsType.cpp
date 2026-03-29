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
    case eGameObjectsType::MISSILE:
        return "MISSILE";
    case eGameObjectsType::ENEMY_MISSILE:
        return "ENEMY_MISSILE";
    case eGameObjectsType::NEUTRAL_SPACE_OBJECT:
        return "NEUTRAL_SPACE_OBJECT";
    case eGameObjectsType::SPACE_STATION:
        return "SPACE_STATION";
    case eGameObjectsType::BARRIER:
        return "BARRIER";
    default:
        ext_assert(false, "Unknown game object type");
        return "UNKNOWN";
    }
}
} // namespace Game
