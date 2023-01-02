#pragma once

#include "UiItemBase.h"

namespace Graphics
{
    namespace Proxy
    {
        class UiSceneProxyBase;
    }
}

namespace EngineCore
{
    class UiCanvas;

    namespace GUI
    {
        class UiLabel : public UiItemBase
        {
            std::string mText;
            
            float mOpacity;

            const std::string mFontName;

        public:
            explicit UiLabel(const std::weak_ptr<UiCanvas> &canvasParent, const std::weak_ptr<IUiTransformable> &parent, const std::string& fontName);

            ~UiLabel() override;

            void SetText(const std::string &text);

            std::string GetText() const;

            void SetOpacity(const float opacity);

            float GetOpacity() const;

            std::string GetFontName() const;

            std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const;

            void OnPropertiesShouldBeUpdatedOnRenderThread() override;

        protected:
            void OnRegistered() override;

            void OnUnregistered() override;

        private:
            void SyncDataOnRenderThread();
        };
    }
}