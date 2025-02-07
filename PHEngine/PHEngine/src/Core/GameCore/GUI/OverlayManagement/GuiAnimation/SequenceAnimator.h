#pragma once

#include "AnimationSequence.h"
#include "Core/GameCore/ITickable.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace EngineCore {
namespace GUI {
class IAnimatable;
class IAnimationController;

class SequenceAnimator : public ITickable {
    std::unordered_map<std::string, std::vector<AnimationSequence>> mAnimationSequences;

    std::string mActiveAnimationSequenceName;

    std::shared_ptr<::EngineCore::GUI::IAnimatable> mAnimatable;

    bool mAnimationInProgress{false};

    std::unordered_map<std::string, std::shared_ptr<::EngineCore::GUI::IAnimationController>> mAnimationControllers;

public:
    explicit SequenceAnimator(const std::shared_ptr<::EngineCore::GUI::IAnimatable>& animatable);

    ~SequenceAnimator() = default;

    void Tick(const float deltaTime) override;

    void UnpausableTick(const float deltaTime) override;

    void AddSequenceAnimation(const std::string& animationName, const AnimationSequence& animationSequence);

    void RemoveAnimation(const std::string& animationName);

    void StartSequenceAnimation(const std::string& newAnimationName);

    bool HasAnimation(const std::string& animationName) const;

private:
    void CreateAnimationControllersForAnimationSequences(const std::string& animationName);

    bool CheckNewAnimationSequenceForValidity(const std::string& animationName, const AnimationSequence& animationSequence) const;
};
} // namespace GUI
} // namespace EngineCore
