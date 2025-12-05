#include "SequenceAnimator.h"

#include "AnimationControllers/AnimationControllerFactory.h"
#include "AnimationControllers/IAnimationController.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GameCore/EnginePropertyType.h"
#include "IAnimatable.h"

#include <algorithm>

namespace EngineCore {
namespace GUI {
SequenceAnimator::SequenceAnimator(const std::shared_ptr<IAnimatable>& animatable)
    : mAnimatable(animatable)
{
    ext_assert(mAnimatable, "SequenceAnimator::SequenceAnimator: animatable is null");
}

void SequenceAnimator::Tick(const float deltaTimeSec)
{
}

void SequenceAnimator::UnpausableTick(const float deltaTimeSec)
{
    if (mAnimationInProgress) {
        ext_assert(
            !mActiveAnimationSequenceName.empty(), "SequenceAnimator::UnpausableTick: activeAnimationSequenceName is empty");
        ext_assert(
            mAnimationSequences.count(mActiveAnimationSequenceName),
            "SequenceAnimator::UnpausableTick: activeAnimationSequenceName not found in animationSequences: "
                + mActiveAnimationSequenceName);
        auto& animationSequences = mAnimationSequences[mActiveAnimationSequenceName];
        bool isAnimationFinished = true;
        for (auto& animationSequence : animationSequences) {
            const auto animationDataIndex = animationSequence.GetCurrentAnimationDataIndex();
            if (animationDataIndex == -1) {
                continue;
            }
            animationSequence.AddDeltaTimeToAnimationTime(deltaTimeSec);
            const auto& animationDataList = animationSequence.GetAnimationDataInSequence();
            ext_assert(
                animationDataIndex > -1 && animationDataIndex < animationDataList.size(),
                "SequenceAnimator::UnpausableTick: animationDataIndex out of range");
            const auto& animationData = animationDataList.at(animationDataIndex);

            ext_assert(
                mAnimationControllers.count(animationData.GetPropertyName()),
                "SequenceAnimator::UnpausableTick: animation controller not found for property: "
                    + animationData.GetPropertyName());
            const auto& propertyController = mAnimationControllers.at(animationData.GetPropertyName());
            propertyController->ProcessAnimation(animationSequence.GetPassedAnimationTime(), animationData, mAnimatable);

            if (propertyController->IsAnimationFinished()) {
                isAnimationFinished = animationSequence.TryToMoveToNextAnimation() ? false : isAnimationFinished;
                if (!isAnimationFinished) {
                    // moved to next animation in sequence, need to initialize with src values
                    const auto nextAnimationDataIndex = animationSequence.GetCurrentAnimationDataIndex();
                    const auto& nextAnimationData = animationDataList.at(nextAnimationDataIndex);
                    ext_assert(
                        mAnimationControllers.count(nextAnimationData.GetPropertyName()),
                        "SequenceAnimator::UnpausableTick: animation controller not found for property: "
                            + nextAnimationData.GetPropertyName());
                    const auto& nextPropertyController = mAnimationControllers.at(nextAnimationData.GetPropertyName());
                    nextPropertyController->InitWithSrcValues(nextAnimationData, mAnimatable);
                }
            } else {
                isAnimationFinished = false;
            }
        }
        if (isAnimationFinished) {
            mAnimationControllers.clear();
            auto& currentAnimationSequences = mAnimationSequences[mActiveAnimationSequenceName];
            std::for_each(currentAnimationSequences.begin(), currentAnimationSequences.end(), [](auto& animationSequence) {
                animationSequence.SetPassedAnimationTime(0.0f);
            });
            mActiveAnimationSequenceName = "";
            mAnimationInProgress = false;
        }
    }
}

void SequenceAnimator::AddSequenceAnimation(const std::string& animationName, const AnimationSequence& animationSequence)
{
    ext_assert(
        CheckNewAnimationSequenceForValidity(animationName, animationSequence),
        "SequenceAnimator::AddSequenceAnimation: animation sequence is not valid for animation name: " + animationName);

    if (mAnimationSequences.count(animationName)) {
        mAnimationSequences.at(animationName).emplace_back(animationSequence);
    } else {
        mAnimationSequences.emplace(animationName, std::vector<AnimationSequence>({animationSequence}));
    }
}

bool SequenceAnimator::CheckNewAnimationSequenceForValidity(
    const std::string& animationName, const AnimationSequence& animationSequence) const
{
    if (mAnimationSequences.count(animationName)) {
        const auto& animationSequencesList = mAnimationSequences.at(animationName);
        const auto bSamePropertyInDifferentSequences = std::any_of(
            animationSequencesList.cbegin(),
            animationSequencesList.cend(),
            [newAnimationSequence = animationSequence](const auto& existingAnimationSequence) {
                const auto& newAnimationDataList = newAnimationSequence.GetAnimationDataInSequence();
                const auto& existingAnimationDataList = existingAnimationSequence.GetAnimationDataInSequence();
                return std::any_of(
                    existingAnimationDataList.cbegin(),
                    existingAnimationDataList.cend(),
                    [&newAnimationDataList](const auto& existingAnimationData) {
                        return std::any_of(
                            newAnimationDataList.cbegin(),
                            newAnimationDataList.cend(),
                            [&existingAnimationData](const auto& newAnimationData) {
                                return newAnimationData.GetPropertyName() == existingAnimationData.GetPropertyName();
                            });
                    });
            });
        return !bSamePropertyInDifferentSequences;
    }
    return true;
}

void SequenceAnimator::RemoveAnimation(const std::string& animationName)
{
    ext_assert(
        mAnimationSequences.count(animationName), "SequenceAnimator::RemoveAnimation: animation not found: " + animationName);
    mAnimationSequences.erase(animationName);
}

void SequenceAnimator::StartSequenceAnimation(const std::string& newAnimationName)
{
    if (mAnimationInProgress) {
        ext_assert(
            !mActiveAnimationSequenceName.empty(),
            "SequenceAnimator::StartSequenceAnimation: activeAnimationSequenceName is empty");
        ext_assert(
            mAnimationSequences.count(mActiveAnimationSequenceName),
            "SequenceAnimator::StartSequenceAnimation: activeAnimationSequenceName not found in animationSequences: "
                + mActiveAnimationSequenceName);
        auto& sequences = mAnimationSequences[mActiveAnimationSequenceName];
        for (auto& sequence : sequences) {
            const auto& animationDataList = sequence.GetAnimationDataInSequence();
            const auto currentDataIndex = sequence.GetCurrentAnimationDataIndex();
            ext_assert(
                currentDataIndex > -1 && currentDataIndex < animationDataList.size(),
                "SequenceAnimator::StartSequenceAnimation: currentDataIndex out of range");
            const auto& animationData = animationDataList.at(currentDataIndex);
            ext_assert(
                mAnimationControllers.count(animationData.GetPropertyName()),
                "SequenceAnimator::StartSequenceAnimation: animation controller not found for property: "
                    + animationData.GetPropertyName());
            const auto& propertyController = mAnimationControllers.at(animationData.GetPropertyName());
            propertyController->ForceFinishAnimation(animationData, mAnimatable);
            sequence.SetCurrentAnimationDataIndex(-1);
        }
        mAnimationControllers.clear();
        mActiveAnimationSequenceName = "";
    }

    ext_assert(
        mAnimationSequences.count(newAnimationName),
        "SequenceAnimator::StartSequenceAnimation: animation not found: " + newAnimationName);
    mActiveAnimationSequenceName = newAnimationName;
    std::for_each(mAnimationSequences.begin(), mAnimationSequences.end(), [](auto& animationSequencePair) {
        auto& animationSequences = animationSequencePair.second;
        for (auto& animationSequence : animationSequences) {
            animationSequence.SetPassedAnimationTime(0.0f);
        }
    });
    mAnimationInProgress = true;
    CreateAnimationControllersForAnimationSequences(newAnimationName);
    for (auto& animationSequence : mAnimationSequences[mActiveAnimationSequenceName]) {
        animationSequence.SetCurrentAnimationDataIndex(0); // Init index for sequence
        const auto& animationDataList = animationSequence.GetAnimationDataInSequence();
        const auto& firstAnimationData = animationDataList.at(0);
        ext_assert(
            mAnimationControllers.count(firstAnimationData.GetPropertyName()),
            "SequenceAnimator::StartSequenceAnimation: animation controller not found for property: "
                + firstAnimationData.GetPropertyName());
        const auto& propertyController = mAnimationControllers.at(firstAnimationData.GetPropertyName());
        propertyController->InitWithSrcValues(firstAnimationData, mAnimatable);
    }
}

bool SequenceAnimator::HasAnimation(const std::string& animationName) const
{
    return mAnimationSequences.count(animationName) > 0;
}

void SequenceAnimator::CreateAnimationControllersForAnimationSequences(const std::string& animationName)
{
    const auto& animationSequences = mAnimationSequences.at(animationName);

    for (const auto& sequence : animationSequences) {
        const auto& animationDataList = sequence.GetAnimationDataInSequence();
        for (const auto& animationData : animationDataList) {
            if (!mAnimationControllers.count(animationData.GetPropertyName())) {
                const auto& property = mAnimatable->GetPropertyByName(animationData.GetPropertyName());
                ext_assert(
                    property,
                    "SequenceAnimator::CreateAnimationControllersForAnimationSequences: property is null for property name: "
                        + animationData.GetPropertyName());
                const auto propertyType = property->GetPropertyType();
                const std::shared_ptr<IAnimationController> propertyController
                    = AnimationControllerFactory::CreateAnimationController(propertyType);
                ext_assert(
                    propertyController,
                    "SequenceAnimator::CreateAnimationControllersForAnimationSequences: propertyController is null for property "
                    "name: "
                        + animationData.GetPropertyName());
                mAnimationControllers.emplace(animationData.GetPropertyName(), propertyController);
            }
        }
    }
}
} // namespace GUI
} // namespace EngineCore
