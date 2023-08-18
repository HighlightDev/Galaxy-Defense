#pragma once

#include <vector>
#include <memory>

#include <glm/vec2.hpp>

namespace EngineCore
{
    class FontHandler;

    namespace GUI
    {
        class UiCanvas;
    }
}

namespace Graphics
{
    namespace Proxy
    {
        class UiSceneProxyBase;

        class UiCanvasSceneProxy
        {
            size_t mUiItemUId;

            bool mIsVisible;

            glm::ivec2 mAbsoluteOrigin;

            glm::ivec2 mWidthHeight;

            std::vector<std::shared_ptr<UiSceneProxyBase>> mUiProxies;

            std::weak_ptr<::EngineCore::FontHandler> mFontHandlerWp;

            float mOverlayOpacity;

            size_t mCanvasZOrder;

        public:
            UiCanvasSceneProxy(const ::EngineCore::GUI::UiCanvas *canvas);

            void CleanUp();

            void AddUiSceneProxy(std::shared_ptr<UiSceneProxyBase> uiProxy);

            void RemoveUiSceneProxy(const size_t uiItemUId);

            void SortProxiesByZOrder();

            void Render();

            void SetUiItemUid(const size_t UId);

            size_t GetUiItemUId() const;

            void SetIsVisible(const bool isVisible);

            bool IsVisible() const;

            void SetAbsoluteOrigin(const glm::ivec2 &position);

            void SetWidthHeight(const glm::ivec2 &widthHeight);

            void SetOverlayOpacity(const float opacity);

            float GetOverlayOpacity() const;

            void SetCanvasZOrder(const size_t zOrder);

            size_t GetCanvasZOrder() const;

            glm::ivec2 GetAbsoluteOrigin() const;

            glm::ivec2 GetWidthHeight() const;

            std::shared_ptr<UiSceneProxyBase> GetSceneProxyById(const size_t uid) const;

            void SetFontHandler(const std::weak_ptr<::EngineCore::FontHandler> &fontHandlerWp);

            std::weak_ptr<::EngineCore::FontHandler> GetFontHandler() const;
        };
    }
}