#pragma once

#include "LuaProxy.h"
#include "IAnimatableLuaProxy.h"

#include <string>

namespace EngineCore
{
    namespace GUI
    {
        class UiCanvas;
        class AnimationData;
    }
}

namespace EngineCore
{
    namespace Scripts
    {
        class UiCanvasLuaProxy
            : public LuaProxy,
              public IAnimatableLuaProxy
        {
            std::string mCanvasName;

            bool mIsVisible;

        public:
            explicit UiCanvasLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiCanvas> &ownerCanvas);

            std::string GetCanvasName() const;

            void SetIsVisible_FromGameThread(const bool isVisible);

            void OnLuaThreadDataUpdated(const std::string &jsonParameters) override;

            std::string GetGameThreadData() override;

            bool IsVisible() const;

            void InitializeInputSystem();

            bool IsAnimationSupported() const override;

            void AddAnimation(const std::string& animationName, const ::EngineCore::GUI::AnimationData &animationData) override;

            void StartAnimation(const std::string& animationName) override;
        };
    }
}
