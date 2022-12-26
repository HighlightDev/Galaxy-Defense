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

            void SetColor(const glm::vec4 &color);

            void SetColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);

            void SetColor(const uint32_t hexColor);

            glm::vec4 GetColor() const;

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