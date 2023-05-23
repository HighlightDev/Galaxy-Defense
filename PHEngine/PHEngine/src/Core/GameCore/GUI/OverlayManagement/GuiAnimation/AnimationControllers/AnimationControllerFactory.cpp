#include "AnimationControllerFactory.h"
#include "FloatAnimationController.h"

namespace EngineCore
{
    namespace GUI
    {
        std::unique_ptr<IAnimationController> AnimationControllerFactory::CreateAnimationController(const eEnginePropertyType propertyType)
        {
            switch (propertyType)
            {
            case eEnginePropertyType::Float:
                return std::make_unique<FloatAnimationController>();

            default:
                return nullptr;
            }
        }
    }
}
