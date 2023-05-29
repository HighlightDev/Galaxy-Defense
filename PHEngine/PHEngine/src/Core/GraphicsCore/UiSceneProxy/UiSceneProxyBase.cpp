#include "UiSceneProxyBase.h"
#include "Core/GraphicsCore/UiSceneProxy/UiCanvasSceneProxy.h"
#include "Core/GameCore/GUI/UiElements/UiItemBase.h"

using namespace EngineCore::GUI;

namespace Graphics
{
    namespace Proxy
    {
        UiSceneProxyBase::UiSceneProxyBase(const UiItemBase *uiItemBase)
            : mUiItemUId(uiItemBase->GetUId()),
              mIsVisible(false),
              mZOrder(uiItemBase->GetZOrder()),
              mParentCanvasProxy(),
              mNormalizedTranslation(uiItemBase->GetNormalizedTranslation()),
              mNormalizedScale(uiItemBase->GetNormalizedScale()),
              mWidthHightPixels(glm::ivec2(static_cast<int32_t>(uiItemBase->GetWidth()), static_cast<int32_t>(uiItemBase->GetHeight()))),
              mCenterOffset(glm::vec2(0.0f, 0.0f))
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

        void UiSceneProxyBase::SetTransform(const glm::vec2 &normalizedTranslation, const glm::vec2 &normalizedScale)
        {
            mNormalizedTranslation = normalizedTranslation;
            mNormalizedScale = normalizedScale;
        }

        void UiSceneProxyBase::SetWidthHeightPixels(const glm::ivec2 &widthHeight)
        {
            mWidthHightPixels = widthHeight;
        }

        void UiSceneProxyBase::SetOverlayOpacity(const float overlayOpacity)
        {
            mOverlayOpacity = overlayOpacity;
        }

        float UiSceneProxyBase::GetOverlayOpacity() const
        {
            return mOverlayOpacity;
        }

        void UiSceneProxyBase::SetScale(const float scale)
        {
            mScale = scale;
        }

        float UiSceneProxyBase::GetScale() const
        {
            return mScale;
        }

        void UiSceneProxyBase::SetCenterOffset(const glm::vec2 &offset)
        {
            mCenterOffset = offset;
        }

        glm::vec2 UiSceneProxyBase::GetCenterOffset() const
        {
            return mCenterOffset;
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