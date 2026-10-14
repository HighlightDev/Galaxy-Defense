#pragma once

#include "AnimationData.h"
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

class Animator : public ITickable {
    std::unordered_map<std::string, std::vector<AnimationData>> mAnimations;

    std::string mActiveAnimationName;

    std::shared_ptr<::EngineCore::GUI::IAnimatable> mAnimatable;

    bool mAnimationInProgress{false};

    float mAnimationTimePassed{0.0f};

    std::vector<std::function<void(std::string)>> mOnAnimationFinishedCallbacks;

    std::unordered_map<std::string, std::shared_ptr<::EngineCore::GUI::IAnimationController>> mAnimationControllers;

    bool bFinishAnimationOnNewAnimationStart{true};

public:
    explicit Animator(const std::shared_ptr<::EngineCore::GUI::IAnimatable>& animatable);

    ~Animator() = default;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override;

    void AddAnimation(const std::string& animationName, const AnimationData& animationData);

    void RemoveAnimation(const std::string& animationName);

    void SubscribeOnAnimationFinished(const std::function<void(std::string)>& callback);

    void StartAnimation(const std::string& newAnimationName);

    bool HasAnimation(const std::string& animationName) const;

    std::string GetActiveAnimationName() const;

    void SetFinishAnimationOnNewAnimationStart(const bool finishOnNewAnimationStart);

    bool GetFinishAnimationOnNewAnimationStart() const;

private:
    void CreateAnimationControllersForAnimation(const std::string& animationName);
};
} // namespace GUI
} // namespace EngineCore
