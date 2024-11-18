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
        class UiProgressBar : public UiItemBase
        {
            glm::vec3 mEmptyColor;

            glm::vec3 mFilledColor;

            float mOpacity;

            float mFillPercentValue;

            std::shared_ptr<EngineObjectProperty<float>> mOpacityProperty;

        public:
            explicit UiProgressBar(const std::string &name = std::string(""));

            ~UiProgressBar() override;

            void SetEmptyColor(const glm::vec3 &color);

            void SetEmptyColor(const uint8_t r, const uint8_t g, const uint8_t b);

            void SetEmptyColor(const uint32_t hexColor);

            void SetFilledColor(const glm::vec3 &color);

            void SetFilledColor(const uint8_t r, const uint8_t g, const uint8_t b);

            void SetFilledColor(const uint32_t hexColor);

            void SetOpacity(const float opacity);

            float GetOpacity() const;

            void SetFillPercentValue(const float value);

            float GetFillPercentValue() const;

            glm::vec3 GetEmptyColor() const;

            glm::vec3 GetFilledColor() const;

            std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const;

            std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

            void OnPropertiesShouldBeUpdatedOnRenderThread() override;

            void OnPropertiesShouldBeUpdatedOnLuaThread() override;

            void SyncFromLuaJsonProperties(const std::string &luaJsonPropsStr) override;

            std::string GetUiTypeString() const override;

        protected:
            void OnRegistered() override;

            void OnUnregistered() override;

        private:
            void SyncDataOnRenderThread();

            void SyncDataOnLuaThread();
        };
    }
}