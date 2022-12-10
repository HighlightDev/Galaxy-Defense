#pragma once

#include <stdint.h>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <glm/vec2.hpp>

namespace EngineCore
{
    namespace GUI
    {
        class UiItemBase;
    }
}

namespace Graphics
{
    namespace Proxy
    {
        class UiCanvasSceneProxy;

        class UiSceneProxyBase
        {
        protected:
            size_t mUiItemUId;

            bool mIsVisible;

            size_t mZOrder;

            std::weak_ptr<UiCanvasSceneProxy> mParentCanvasProxy;

            glm::vec2 mNormalizedTranslation, mNormalizedScale;

        public:
            UiSceneProxyBase(const ::EngineCore::GUI::UiItemBase* uiItemBase);

            void SetUiItemUid(const size_t UId);

            size_t GetUiItemUId() const;

            void SetIsVisible(const bool isVisible);

            bool IsVisible() const;

            void SetZOrder(const size_t zOrder);

            size_t GetZOrder() const;

            void SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale);

            void SetCanvasSceneProxy(const std::weak_ptr<UiCanvasSceneProxy> &parentCanvasProxy);

            virtual void Render();
        };
    }
}