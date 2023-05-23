#pragma once

#include <memory>

#include "Core/GameCore/EnginePropertyType.h"

namespace EngineCore
{
    namespace GUI
    {
        class AnimationData;
        class IAnimatable;

        class IAnimationController
        {
        protected:
            bool mIsAnimationFinished{false};

        public:
            virtual void ProcessAnimation(const float animationTimePassed,
                                          const ::EngineCore::GUI::AnimationData &data,
                                          const std::shared_ptr<::EngineCore::GUI::IAnimatable> &animatable) = 0;

            virtual void ForceFinishAnimation(const ::EngineCore::GUI::AnimationData &data,
                                              const std::shared_ptr<::EngineCore::GUI::IAnimatable> &animatable) = 0;

            virtual void InitWithSrcValues(const ::EngineCore::GUI::AnimationData &data,
                                           const std::shared_ptr<::EngineCore::GUI::IAnimatable> &animatable) = 0;

            virtual eEnginePropertyType GetPropertyType() const = 0;

            bool IsAnimationFinished() const
            {
                return mIsAnimationFinished;
            }

            void Reset()
            {
                mIsAnimationFinished = false;
            }
        };
    }
}
