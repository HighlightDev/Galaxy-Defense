#include "UiSceneProxyBase.h"
#include "Core/GraphicsCore/UiSceneProxy/UiCanvasSceneProxy.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"

using namespace EngineCore::GUI;

namespace Graphics
{
    namespace Proxy
    {
        UiSceneProxyBase::UiSceneProxyBase(const UiItemBase* uiItemBase)
            : mUiItemUId(uiItemBase->GetUId()),
              mIsVisible(uiItemBase->IsVisible()),
              mZOrder(uiItemBase->GetZOrder()),
              mParentCanvasProxy(),
              mNormalizedTranslation(uiItemBase->GetNormalizedTranslation()),
              mNormalizedScale(uiItemBase->GetNormalizedScale())
        {
        }

        void UiSceneProxyBase::SetUiItemUid(const size_t UId)
        {
            mUiItemUId = UId;
        }

        size_t UiSceneProxyBase::GetUiItemUId() const
        {
            return mUiItemUId;
        }

        void UiSceneProxyBase::SetIsVisible(const bool isVisible)
        {
            mIsVisible = isVisible;
        }

        bool UiSceneProxyBase::IsVisible() const
        {
            return mIsVisible;
        }

        void UiSceneProxyBase::SetZOrder(const size_t zOrder)
        {
            if (const auto &canvasSp = mParentCanvasProxy.lock())
            {
                mZOrder = zOrder;
                canvasSp->SortProxiesByZOrder();
            }
        }

        size_t UiSceneProxyBase::GetZOrder() const
        {
            return mZOrder;
        }

        void UiSceneProxyBase::SetTransform(const glm::vec2& normalizedTranslation, const glm::vec2& normalizedScale)
        {
            mNormalizedTranslation = normalizedTranslation;
            mNormalizedScale = normalizedScale;
        }

        void UiSceneProxyBase::SetCanvasSceneProxy(const std::weak_ptr<UiCanvasSceneProxy> &parentCanvasProxy)
        {
            mParentCanvasProxy = parentCanvasProxy;
        }

        void UiSceneProxyBase::Render()
        {
        }
    }
}