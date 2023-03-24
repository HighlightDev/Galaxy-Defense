#pragma once

#include "UiItemBase.h"

#include <glm/vec3.hpp>

namespace Graphics
{
    namespace Proxy
    {
        class UiSceneProxyBase;
    }
}

namespace EngineCore
{
    namespace Scripts
    {
        class LuaProxy;
    }
}

namespace EngineCore
{
    class UiCanvas;
    namespace GUI
    {
        class UiRectangle : public UiItemBase
        {
            glm::vec3 mColor;

            float mOpacity;

        public:
            UiRectangle();

            ~UiRectangle() override;

            void SetColor(const glm::vec3 &color);

            void SetColor(const uint8_t r, const uint8_t g, const uint8_t b);

            void SetColor(const uint32_t hexColor);

            void SetOpacity(const float opacity);

            float GetOpacity() const;

            glm::vec3 GetColor() const;

            std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const;

            std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

            void OnPropertiesShouldBeUpdatedOnRenderThread() override;

        protected:
            void OnRegistered() override;

            void OnUnregistered() override;

        private:
            void SyncDataOnRenderThread();
        };
    }
}