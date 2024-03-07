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
        class AnimationSequence;
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

            bool mCanInterceptMouseInputEvents;

            size_t mCanvasZOrder;

        public:
            explicit UiCanvasLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiCanvas> &ownerCanvas);

            void CleanUp() override;

            std::string GetCanvasName() const;

            void SetIsVisible_FromGameThread(const bool isVisible);

            void SetIfCanInterceptMouseInputEvents_FromGameThread(const bool intercepts);

            void SetCanvasZOrder_FromGameThread(const size_t zOrder);

            size_t GetCanvasZOrder() const;

            void OnLuaThreadDataUpdated(const std::string &jsonParameters) override;

            std::string GetGameThreadData() override;

            bool IsVisible() const;

            bool GetIfCanInterceptMouseInputEvents() const;

            void InitializeInputSystem();

            void AddAnimation(const std::string &animationName, const ::EngineCore::GUI::AnimationData &animationData) override;

            void AddSequenceAnimation(const std::string& animationName, const ::EngineCore::GUI::AnimationSequence& animationSequence) override;

            void StartAnimation(const std::string &animationName) override;

            void StartSequenceAnimation(const std::string& animationSequenceName) override;
        };
    }
}
