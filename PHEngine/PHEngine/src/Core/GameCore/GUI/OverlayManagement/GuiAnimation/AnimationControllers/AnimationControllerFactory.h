#pragma once

#include "Core/GameCore/EnginePropertyType.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/AnimationControllers/IAnimationController.h"

#include <memory>

namespace EngineCore
{
    namespace GUI
    {
        class AnimationControllerFactory
        {
        public:
            static std::unique_ptr<IAnimationController> CreateAnimationController(const eEnginePropertyType propertyType);
        };
    } // namespace GUI
}
