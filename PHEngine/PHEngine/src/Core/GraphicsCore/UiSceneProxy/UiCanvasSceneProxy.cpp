#include "UiCanvasSceneProxy.h"
#include "Core/GraphicsCore/UiSceneProxy/UiSceneProxyBase.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/Common/FontHandler.h"

#include <algorithm>

using namespace EngineCore::GUI;
using namespace EngineCore;

namespace Graphics
{
    namespace Proxy
    {
        UiCanvasSceneProxy::UiCanvasSceneProxy(const UiCanvas *canvas)
            : mUiItemUId(canvas->GetUId()),
              mIsVisible(false),
              mAbsoluteOrigin(canvas->GetAbsoluteOrigin()),
              mWidthHeight(glm::ivec2(canvas->GetWidth(), canvas->GetHeight())),
              mFontHandlerWp()
        {
        }

        void UiCanvasSceneProxy::AddUiSceneProxy(std::shared_ptr<UiSceneProxyBase> uiProxy)
        {
            mUiProxies.emplace_back(uiProxy);
        }

        void UiCanvasSceneProxy::RemoveUiSceneProxy(const std::shared_ptr<UiSceneProxyBase> &uiProxy)
        {
            mUiProxies.erase(std::remove_if(mUiProxies.begin(), mUiProxies.end(), [&](const auto &proxy)
                                            { return uiProxy->GetUiItemUId() == proxy->GetUiItemUId(); }));
        }

        void UiCanvasSceneProxy::SortProxiesByZOrder()
        {
            std::sort(mUiProxies.begin(), mUiProxies.end(), [](const auto &left, const auto &right)
                      { return left->GetZOrder() < right->GetZOrder(); });
        }

        void UiCanvasSceneProxy::Render()
        {
            if (mIsVisible)
            {
                for (const auto &proxy : mUiProxies)
                {
                    if (proxy->IsVisible())
                    {
                        proxy->Render();
                    }
                }
            }
        }

        void UiCanvasSceneProxy::SetUiItemUid(const size_t UId)
        {
            mUiItemUId = UId;
        }

        size_t UiCanvasSceneProxy::GetUiItemUId() const
        {
            return mUiItemUId;
        }

        void UiCanvasSceneProxy::SetIsVisible(const bool isVisible)
        {
            if (mIsVisible != isVisible)
            {
                mIsVisible = isVisible;
            }
        }

        bool UiCanvasSceneProxy::IsVisible() const
        {
            return mIsVisible;
        }

        void UiCanvasSceneProxy::SetAbsoluteOrigin(const glm::ivec2 &position)
        {
            mAbsoluteOrigin = position;
        }

        void UiCanvasSceneProxy::SetWidthHeight(const glm::ivec2 &widthHeight)
        {
            mWidthHeight = widthHeight;
        }

        glm::ivec2 UiCanvasSceneProxy::GetAbsoluteOrigin() const
        {
            return mAbsoluteOrigin;
        }

        glm::ivec2 UiCanvasSceneProxy::GetWidthHeight() const
        {
            return mWidthHeight;
        }

        std::shared_ptr<UiSceneProxyBase> UiCanvasSceneProxy::GetSceneProxyById(const size_t uid) const
        {
           for (const auto proxy : mUiProxies)
           {
              if (uid == proxy->GetUiItemUId())
                 return proxy;
           }
           return nullptr;
        }

        void UiCanvasSceneProxy::SetFontHandler(const std::weak_ptr<::EngineCore::FontHandler> &fontHandlerWp)
        {
            mFontHandlerWp = fontHandlerWp;
        }

        std::weak_ptr<::EngineCore::FontHandler> UiCanvasSceneProxy::GetFontHandler() const
        {
            return mFontHandlerWp;
        }
    }
}