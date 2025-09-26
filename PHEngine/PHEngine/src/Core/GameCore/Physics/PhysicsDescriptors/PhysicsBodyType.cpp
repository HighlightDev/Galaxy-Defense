#include "PhysicsBodyType.h"

namespace EnginePhysics {

std::string ToString(const ePhysicsBodyType bodyType)
{
    switch (bodyType) {
    case ePhysicsBodyType::STATIC:
        return "STATIC";
    case ePhysicsBodyType::DYNAMIC:
        return "DYNAMIC";
    case ePhysicsBodyType::KINEMATIC:
        return "KINEMATIC";
    case ePhysicsBodyType::GHOST:
        return "GHOST";
    default:
        return "UNKNOWN";
    }
}

std::string ToString(const ePhysicsDescriptorType descriptorType)
{
    switch (descriptorType) {
    case ePhysicsDescriptorType::UNDEFINED:
        return "UNDEFINED";
    case ePhysicsDescriptorType::RIGID_BODY_CONTROLLER:
        return "RIGID_BODY_CONTROLLER";
    case ePhysicsDescriptorType::DYNAMIC_CHARACTER_CONTROLLER:
        return "DYNAMIC_CHARACTER_CONTROLLER";
    case ePhysicsDescriptorType::GHOST_CONTROLLER:
        return "GHOST_CONTROLLER";
    default:
        return "UNKNOWN";
    }
}

} // namespace EnginePhysics