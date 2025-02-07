#pragma once

#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/Animator.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/SequenceAnimator.h"

#include <vector>

using namespace EngineCore;

namespace EngineCore {
namespace GUI {
class AnimationData;
class AnimationSequence;

class IAnimatable {
public:
    virtual std::shared_ptr<EngineObjectPropertyBase> GetPropertyByName(const std::string& propName) const = 0;

    virtual std::shared_ptr<Animator> GetAnimator() const = 0;

    virtual std::shared_ptr<SequenceAnimator> GetSequenceAnimator() const = 0;

    virtual void CreateAnimator() = 0;

    virtual void CreateSequenceAnimator() = 0;

    virtual void AddAnimation(const std::string& animationName, const ::EngineCore::GUI::AnimationData& animationData) = 0;

    virtual void
    AddSequenceAnimation(const std::string& animationName, const ::EngineCore::GUI::AnimationSequence& animationSequence)
        = 0;
};
} // namespace GUI
} // namespace EngineCore
