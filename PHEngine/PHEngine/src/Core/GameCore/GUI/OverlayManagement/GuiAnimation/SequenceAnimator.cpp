#include "SequenceAnimator.h"
#include "IAnimatable.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GameCore/EnginePropertyType.h"
#include "Core/CommonCore/Assertion.h"
#include "AnimationControllers/AnimationControllerFactory.h"
#include "AnimationControllers/IAnimationController.h"

#include <algorithm>

namespace EngineCore
{
    namespace GUI
    {
        SequenceAnimator::SequenceAnimator(const std::shared_ptr<IAnimatable> &animatable)
            : mAnimatable(animatable)
        {
            assert(mAnimatable);
        }

        void SequenceAnimator::Tick(const float deltaTime)
        {
        }

        void SequenceAnimator::UnpausableTick(const float deltaTime)
        {
            if (mAnimationInProgress)
            {
                assert(!mActiveAnimationSequenceName.empty());
                assert(mAnimationSequences.count(mActiveAnimationSequenceName));
                auto &animationSequences = mAnimationSequences[mActiveAnimationSequenceName];
                bool isAnimationFinished = true;
                for (auto &animationSequence : animationSequences)
                {
                    const auto animationDataIndex = animationSequence.GetCurrentAnimationDataIndex();
                    if (animationDataIndex == -1)
                    {
                        continue;
                    }
                    animationSequence.AddDeltaTimeToAnimationTime(deltaTime);
                    const auto &animationDataList = animationSequence.GetAnimationDataInSequence();
                    assert(animationDataIndex > -1 && animationDataIndex < animationDataList.size());
                    const auto &animationData = animationDataList.at(animationDataIndex);

                    assert(mAnimationControllers.count(animationData.GetPropertyName()));
                    const auto &propertyController = mAnimationControllers.at(animationData.GetPropertyName());
                    propertyController->ProcessAnimation(animationSequence.GetPassedAnimationTime(), animationData, mAnimatable);

                    if (propertyController->IsAnimationFinished())
                    {
                        isAnimationFinished = animationSequence.TryToMoveToNextAnimation() ? false : isAnimationFinished;
                        if (!isAnimationFinished)
                        {
                            // moved to next animation in sequence, need to initialize with src values
                            const auto nextAnimationDataIndex = animationSequence.GetCurrentAnimationDataIndex();
                            const auto &nextAnimationData = animationDataList.at(nextAnimationDataIndex);
                            assert(mAnimationControllers.count(nextAnimationData.GetPropertyName()));
                            const auto &nextPropertyController = mAnimationControllers.at(nextAnimationData.GetPropertyName());
                            nextPropertyController->InitWithSrcValues(nextAnimationData, mAnimatable);
                        }
                    }
                    else
                    {
                        isAnimationFinished = false;
                    }
                }
                if (isAnimationFinished)
                {
                    mAnimationControllers.clear();
                    auto &currentAnimationSequences = mAnimationSequences[mActiveAnimationSequenceName];
                    std::for_each(currentAnimationSequences.begin(), currentAnimationSequences.end(), [](auto &animationSequence)
                                  { animationSequence.SetPassedAnimationTime(0.0f); });
                    mActiveAnimationSequenceName = "";
                    mAnimationInProgress = false;
                }
            }
        }

        void SequenceAnimator::AddSequenceAnimation(const std::string &animationName, const AnimationSequence &animationSequence)
        {
            assert(CheckNewAnimationSequenceForValidity(animationName, animationSequence));

            if (mAnimationSequences.count(animationName))
            {
                mAnimationSequences.at(animationName).emplace_back(animationSequence);
            }
            else
            {
                mAnimationSequences.emplace(animationName, std::vector<AnimationSequence>({animationSequence}));
            }
        }

        bool SequenceAnimator::CheckNewAnimationSequenceForValidity(const std::string &animationName, const AnimationSequence &animationSequence) const
        {
            if (mAnimationSequences.count(animationName))
            {
                const auto &animationSequencesList = mAnimationSequences.at(animationName);
                const auto bSamePropertyInDifferentSequences = std::any_of(animationSequencesList.cbegin(), animationSequencesList.cend(), [newAnimationSequence = animationSequence](const auto &existingAnimationSequence) {
                    const auto& newAnimationDataList = newAnimationSequence.GetAnimationDataInSequence();
                    const auto& existingAnimationDataList = existingAnimationSequence.GetAnimationDataInSequence();
                    return std::any_of(existingAnimationDataList.cbegin(), existingAnimationDataList.cend(), [&newAnimationDataList](const auto& existingAnimationData) {
                        return std::any_of(newAnimationDataList.cbegin(), newAnimationDataList.cend(), [&existingAnimationData](const auto& newAnimationData) {
                            return newAnimationData.GetPropertyName() == existingAnimationData.GetPropertyName();
                        });
                    });
                });
                return !bSamePropertyInDifferentSequences;
            }
            return true;
        }

        void SequenceAnimator::RemoveAnimation(const std::string &animationName)
        {
            assert(mAnimationSequences.count(animationName));
            mAnimationSequences.erase(animationName);
        }

        void SequenceAnimator::StartSequenceAnimation(const std::string &newAnimationName)
        {
            if (mAnimationInProgress)
            {
                assert(!mActiveAnimationSequenceName.empty());
                assert(mAnimationSequences.count(mActiveAnimationSequenceName));
                auto &sequences = mAnimationSequences[mActiveAnimationSequenceName];
                for (auto &sequence : sequences)
                {
                    const auto &animationDataList = sequence.GetAnimationDataInSequence();
                    const auto currentDataIndex = sequence.GetCurrentAnimationDataIndex();
                    assert(currentDataIndex > -1 && currentDataIndex < animationDataList.size());
                    const auto &animationData = animationDataList.at(currentDataIndex);
                    assert(mAnimationControllers.count(animationData.GetPropertyName()));
                    const auto &propertyController = mAnimationControllers.at(animationData.GetPropertyName());
                    propertyController->ForceFinishAnimation(animationData, mAnimatable);
                    sequence.SetCurrentAnimationDataIndex(-1);
                }
                mAnimationControllers.clear();
                mActiveAnimationSequenceName = "";
            }

            assert(mAnimationSequences.count(newAnimationName));
            mActiveAnimationSequenceName = newAnimationName;
            std::for_each(mAnimationSequences.begin(), mAnimationSequences.end(), [](auto &animationSequencePair)
                          { 
                            auto& animationSequences = animationSequencePair.second;
                            for (auto& animationSequence : animationSequences)
                            {
                                animationSequence.SetPassedAnimationTime(0.0f);
                            } });
            mAnimationInProgress = true;
            CreateAnimationControllersForAnimationSequences(newAnimationName);
            for (auto &animationSequence : mAnimationSequences[mActiveAnimationSequenceName])
            {
                animationSequence.SetCurrentAnimationDataIndex(0); // Init index for sequence
                const auto &animationDataList = animationSequence.GetAnimationDataInSequence();
                const auto &firstAnimationData = animationDataList.at(0);
                assert(mAnimationControllers.count(firstAnimationData.GetPropertyName()));
                const auto &propertyController = mAnimationControllers.at(firstAnimationData.GetPropertyName());
                propertyController->InitWithSrcValues(firstAnimationData, mAnimatable);
            }
        }

        bool SequenceAnimator::HasAnimation(const std::string &animationName) const
        {
            return mAnimationSequences.count(animationName) > 0;
        }

        void SequenceAnimator::CreateAnimationControllersForAnimationSequences(const std::string &animationName)
        {
            const auto &animationSequences = mAnimationSequences.at(animationName);

            for (const auto &sequence : animationSequences)
            {
                const auto &animationDataList = sequence.GetAnimationDataInSequence();
                for (const auto &animationData : animationDataList)
                {
                    if (!mAnimationControllers.count(animationData.GetPropertyName()))
                    {
                        const auto &property = mAnimatable->GetPropertyByName(animationData.GetPropertyName());
                        assert(property);
                        const auto propertyType = property->GetPropertyType();
                        const std::shared_ptr<IAnimationController> propertyController = AnimationControllerFactory::CreateAnimationController(propertyType);
                        assert(propertyController);
                        mAnimationControllers.emplace(animationData.GetPropertyName(), propertyController);
                    }
                }
            }
        }
    }
}
