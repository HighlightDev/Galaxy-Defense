#include "Animator.h"
#include "IAnimatable.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GameCore/EnginePropertyType.h"
#include "Core/CommonCore/Assertion.h"
#include "AnimationControllers/AnimationControllerFactory.h"
#include "AnimationControllers/IAnimationController.h"

namespace EngineCore
{
    namespace GUI
    {
        Animator::Animator(const std::shared_ptr<IAnimatable> &animatable)
            : mAnimatable(animatable)
        {
            assert(mAnimatable);
        }

        Animator::~Animator()
        {
        }

        void Animator::Tick(const float deltaTime)
        {
        }

        void Animator::UnpausableTick(const float deltaTime)
        {
            if (mAnimationInProgress)
            {
                mAnimationTimePassed += deltaTime;
                assert(!mActiveAnimationName.empty());
                assert(mAnimations.count(mActiveAnimationName));
                assert(mAnimationController);
                const auto &animationData = mAnimations.at(mActiveAnimationName);
                mAnimationController->ProcessAnimation(mAnimationTimePassed, animationData, mAnimatable);
                if (mAnimationController->IsAnimationFinished())
                {
                    for (const auto &animationFinishedCallback : mOnAnimationFinishedCallbacks)
                    {
                        animationFinishedCallback(mActiveAnimationName);
                    }
                    mAnimationController->Reset();
                    mActiveAnimationName = "";
                    mAnimationTimePassed = 0.0f;
                    mAnimationInProgress = false;
                }
            }
        }

        void Animator::AddAnimation(const std::string &animationName, const AnimationData &animationData)
        {
            assert(!mAnimations.count(animationName));
            mAnimations.emplace(animationName, animationData);
        }

        void Animator::RemoveAnimation(const std::string &animationName)
        {
            assert(mAnimations.count(animationName));
            mAnimations.erase(animationName);
        }

        void Animator::SubscribeOnAnimationFinished(const std::function<void(std::string)> &callback)
        {
            mOnAnimationFinishedCallbacks.emplace_back(callback);
        }

        void Animator::StartAnimation(const std::string &newAnimationName)
        {
            if (mAnimationInProgress)
            {
                assert(!mActiveAnimationName.empty());
                assert(mAnimations.count(mActiveAnimationName));
                assert(mAnimationController);
                mAnimationController->ForceFinishAnimation(mAnimations.at(mActiveAnimationName), mAnimatable);
                for (const auto &animationFinishedCallback : mOnAnimationFinishedCallbacks)
                {
                    animationFinishedCallback(mActiveAnimationName);
                }
                mAnimationController->Reset();
                mActiveAnimationName = "";
            }

            assert(mAnimations.count(newAnimationName));
            const auto &property = mAnimatable->GetPropertyByName(mAnimations.at(newAnimationName).GetPropertyName());
            assert(property);
            const auto propertyType = property->GetPropertyType();
            mActiveAnimationName = newAnimationName;
            mAnimationTimePassed = 0.0f;
            mAnimationInProgress = true;
            if (!mAnimationController || (mAnimationController && propertyType != mAnimationController->GetPropertyType()))
            {
                mAnimationController = AnimationControllerFactory::CreateAnimationController(propertyType);
                assert(mAnimationController);
            }
            mAnimationController->InitWithSrcValues(mAnimations.at(mActiveAnimationName), mAnimatable);
        }

        bool Animator::HasAnimation(const std::string &animationName) const
        {
            return mAnimations.count(animationName) > 0;
        }
    }
}
