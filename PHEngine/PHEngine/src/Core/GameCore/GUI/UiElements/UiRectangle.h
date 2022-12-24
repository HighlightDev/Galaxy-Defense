#pragma once

#include "UiItemBase.h"

#include <glm/vec4.hpp>

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
        class UiRectangle : public UiItemBase
        {
            glm::vec4 mColor;

        public:
            explicit UiRectangle(const std::weak_ptr<UiCanvas> &canvasParent, const std::weak_ptr<IUiTransformable> &parent);

            ~UiRectangle() override;

            void SetColor(const glm::vec4& color);

            glm::vec4 GetColor() const;

            std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const;

        protected:
            virtual void OnRegistered() override;

            virtual void OnUnregistered() override;

        private:
            void SyncDataOnRenderThread();
        };
    }
}