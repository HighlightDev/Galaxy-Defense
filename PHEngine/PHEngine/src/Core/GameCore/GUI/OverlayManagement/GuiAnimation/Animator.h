#pragma once

#include "AnimationData.h"
#include "Core/GameCore/ITickable.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <functional>

namespace EngineCore
{
    namespace GUI
    {
        class IAnimatable;
        class IAnimationController;

        class Animator : public ITickable
        {
            std::unordered_map<std::string, AnimationData> mAnimations;

            std::string mActiveAnimationName;

            std::shared_ptr<::EngineCore::GUI::IAnimatable> mAnimatable;

            bool mAnimationInProgress{false};

            float mAnimationTimePassed{0.0f};

            std::function<void(std::string)> mOnAnimationFinishedCallback;

            std::unique_ptr<::EngineCore::GUI::IAnimationController> mAnimationController;

        public:
            explicit Animator(const std::shared_ptr<::EngineCore::GUI::IAnimatable> &animatable);

            ~Animator();

            void Tick(const float deltaTime) override;

            void UnpausableTick(const float deltaTime) override;

            void AddAnimation(const std::string& animationName, const AnimationData& animationData);

            void RemoveAnimation(const std::string& animationName);

            void SubscribeOnAnimationFinished(const std::function<void(std::string)>& callback);

            void StartAnimation(const std::string& newAnimationName);
        };
    }
}
