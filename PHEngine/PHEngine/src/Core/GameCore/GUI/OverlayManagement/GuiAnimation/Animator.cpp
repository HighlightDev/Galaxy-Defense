#include "Animator.h"

#include "AnimationControllers/AnimationControllerFactory.h"
#include "AnimationControllers/IAnimationController.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GameCore/EnginePropertyType.h"
#include "IAnimatable.h"

namespace EngineCore {
namespace GUI {
Animator::Animator(const std::shared_ptr<IAnimatable>& animatable)
    : mAnimatable(animatable)
{
    assert(mAnimatable);
}

void Animator::Tick(const float deltaTimeSec)
{
}

void Animator::UnpausableTick(const float deltaTimeSec)
{
    if (mAnimationInProgress) {
        mAnimationTimePassed += deltaTimeSec;
        assert(!mActiveAnimationName.empty());
        assert(mAnimations.count(mActiveAnimationName));
        bool isAnimationFinished = true;
        for (const auto& animationData : mAnimations.at(mActiveAnimationName)) {
            assert(mAnimationControllers.count(animationData.GetPropertyName()));
            const auto& propertyController = mAnimationControllers.at(animationData.GetPropertyName());
            if (!propertyController->IsAnimationFinished()) {
                propertyController->ProcessAnimation(mAnimationTimePassed, animationData, mAnimatable);
                isAnimationFinished = !propertyController->IsAnimationFinished() ? false : isAnimationFinished;
            }
        }
        if (isAnimationFinished) {
            for (const auto& animationFinishedCallback : mOnAnimationFinishedCallbacks) {
                animationFinishedCallback(mActiveAnimationName);
            }
            mAnimationControllers.clear();
            mActiveAnimationName = "";
            mAnimationTimePassed = 0.0f;
            mAnimationInProgress = false;
        }
    }
}

void Animator::AddAnimation(const std::string& animationName, const AnimationData& animationData)
{
    if (mAnimations.count(animationName)) {
        mAnimations.at(animationName).emplace_back(animationData);
    } else {
        mAnimations.emplace(animationName, std::vector<AnimationData>({animationData}));
    }
}

void Animator::RemoveAnimation(const std::string& animationName)
{
    assert(mAnimations.count(animationName));
    mAnimations.erase(animationName);
}

void Animator::SubscribeOnAnimationFinished(const std::function<void(std::string)>& callback)
{
    mOnAnimationFinishedCallbacks.emplace_back(callback);
}

void Animator::SetFinishAnimationOnNewAnimationStart(const bool finishOnNewAnimationStart)
{
    bFinishAnimationOnNewAnimationStart = finishOnNewAnimationStart;
}

bool Animator::GetFinishAnimationOnNewAnimationStart() const
{
    return bFinishAnimationOnNewAnimationStart;
}

void Animator::StartAnimation(const std::string& newAnimationName)
{
    if (mAnimationInProgress) {
        assert(!mActiveAnimationName.empty());
        assert(mAnimations.count(mActiveAnimationName));
        if (bFinishAnimationOnNewAnimationStart) {
            for (const auto& animationData : mAnimations.at(mActiveAnimationName)) {
                assert(mAnimationControllers.count(animationData.GetPropertyName()));
                const auto& propertyController = mAnimationControllers.at(animationData.GetPropertyName());
                propertyController->ForceFinishAnimation(animationData, mAnimatable);
            }
            for (const auto& animationFinishedCallback : mOnAnimationFinishedCallbacks) {
                animationFinishedCallback(mActiveAnimationName);
            }
        }
        mAnimationControllers.clear();
        mActiveAnimationName = "";
    }

    assert(mAnimations.count(newAnimationName));
    mActiveAnimationName = newAnimationName;
    mAnimationTimePassed = 0.0f;
    mAnimationInProgress = true;
    CreateAnimationControllersForAnimation(newAnimationName);
    if (bFinishAnimationOnNewAnimationStart) {
        for (const auto& animationData : mAnimations.at(mActiveAnimationName)) {
            assert(mAnimationControllers.count(animationData.GetPropertyName()));
            const auto& propertyController = mAnimationControllers.at(animationData.GetPropertyName());
            propertyController->InitWithSrcValues(animationData, mAnimatable);
        }
    }
}

bool Animator::HasAnimation(const std::string& animationName) const
{
    return mAnimations.count(animationName) > 0;
}

void Animator::CreateAnimationControllersForAnimation(const std::string& animationName)
{
    for (const auto& animationData : mAnimations.at(animationName)) {
        const auto& property = mAnimatable->GetPropertyByName(animationData.GetPropertyName());
        assert(property);
        const auto propertyType = property->GetPropertyType();
        const std::shared_ptr<IAnimationController> propertyController
            = AnimationControllerFactory::CreateAnimationController(propertyType);
        assert(propertyController);
        mAnimationControllers.emplace(animationData.GetPropertyName(), propertyController);
    }
}

std::string Animator::GetActiveAnimationName() const
{
    return mActiveAnimationName;
}
} // namespace GUI
} // namespace EngineCore
