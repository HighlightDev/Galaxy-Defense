#pragma once

#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/Animator.h"

using namespace EngineCore;

namespace EngineCore
{
    namespace GUI
    {
        class IAnimatable
        {
        public:
            virtual std::shared_ptr<EngineObjectPropertyBase> GetPropertyByName(const std::string &propName) const = 0;

            virtual std::shared_ptr<Animator> CreateAndGetAnimator() = 0;
        };
    }
}
