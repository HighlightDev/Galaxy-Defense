#include "UiCanvasSceneProxy.h"
#include "Core/GraphicsCore/UiSceneProxy/UiSceneProxyBase.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"

#include <algorithm>

using namespace EngineCore::GUI;

namespace Graphics
{
    namespace Proxy
    {
        UiCanvasSceneProxy::UiCanvasSceneProxy(const UiCanvas *canvas)
            : mUiItemUId(canvas->GetUId()),
              mIsVisible(canvas->IsVisible()),
              mAbsoluteOrigin(canvas->GetAbsoluteOrigin().Translation),
              mWidthHeight(glm::ivec2(canvas->GetWidth(), canvas->GetHeight()))
        {
        }

        void UiCanvasSceneProxy::AddUiSceneProxy(const std::shared_ptr<UiSceneProxyBase> &uiProxy)
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
            for (const auto &proxy : mUiProxies)
            {
                proxy->Render();
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
    }
}