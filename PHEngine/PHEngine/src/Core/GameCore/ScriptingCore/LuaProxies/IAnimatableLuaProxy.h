#pragma once

namespace EngineCore
{
    namespace GUI
    {
        class AnimationData;
    }
}

namespace EngineCore
{
    namespace Scripts
    {
        class IAnimatableLuaProxy
        {
            public:
            virtual bool IsAnimationSupported() const = 0;

            virtual void AddAnimation(const std::string& animationName, const ::EngineCore::GUI::AnimationData& animationData) = 0;

            virtual void StartAnimation(const std::string& animationName) = 0;
        };
    }
}
