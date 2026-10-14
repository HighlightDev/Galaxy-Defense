#include "AnimationControllerFactory.h"

#include "FloatAnimationController.h"
#include "IntegerAnimationController.h"
#include "Vec2AnimationController.h"
#include "Vec3AnimationController.h"

namespace EngineCore {
namespace GUI {
std::unique_ptr<IAnimationController>
AnimationControllerFactory::CreateAnimationController(const eEnginePropertyType propertyType)
{
    switch (propertyType) {
    case eEnginePropertyType::Float:
        return std::make_unique<FloatAnimationController>();
    case eEnginePropertyType::Integer:
        return std::make_unique<IntegerAnimationController>();
    case eEnginePropertyType::Vec3:
        return std::make_unique<Vec3AnimationController>();
    case eEnginePropertyType::Vec2:
        return std::make_unique<Vec2AnimationController>();

    default:
        return nullptr;
    }
}
} // namespace GUI
} // namespace EngineCore
