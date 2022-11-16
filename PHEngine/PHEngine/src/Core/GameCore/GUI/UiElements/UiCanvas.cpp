#include "UiCanvas.h"

#include <algorithm>

namespace EngineCore
{
    namespace GUI
    {
        UiCanvas::UiCanvas(const ViewPortInfo &canvasScreenProperties)
            : mAbsoluteOrigin(glm::ivec2(canvasScreenProperties.OriginX, canvasScreenProperties.OriginY)),
              mRelativeOrigin(mAbsoluteOrigin),
              mWidthHeight(glm::ivec2(canvasScreenProperties.Width, canvasScreenProperties.Height)),
              mChildren(),
              mRegisteredUiItems()
        {
        }

        const Transform2D &UiCanvas::GetAbsoluteOrigin() const
        {
            return mAbsoluteOrigin;
        }

        const Transform2D &UiCanvas::GetRelativeOrigin() const
        {
            return mRelativeOrigin;
        }

        size_t UiCanvas::GetZOrder() const
        {
            return 0;
        }

        size_t UiCanvas::GetWidth() const
        {
            return mWidthHeight.x;
        }

        size_t UiCanvas::GetHeight() const
        {
            return mWidthHeight.y;
        }

        void UiCanvas::SetAbsoluteOrigin(const Transform2D &transform)
        {
            mAbsoluteOrigin = transform;
        }

        void UiCanvas::SetRelativeOrigin(const Transform2D &transform)
        {
        }

        void UiCanvas::SetZOrder(const size_t z_order)
        {
        }

        void UiCanvas::SetWidth(const size_t width)
        {
            mWidthHeight.x = width;
        }

        void UiCanvas::SetHeight(const size_t height)
        {
            mWidthHeight.y = height;
        }

        void UiCanvas::AddUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            RegisterUiItem(uiItem->GetId());
            mChildren.emplace_back(uiItem);
        }

        void UiCanvas::RegisterUiItem(const size_t uiId)
        {
            assert(!mRegisteredUiItems.count(uiId));
            mRegisteredUiItems.insert(uiId);
        }

        void UiCanvas::UnregisterUiItem(const size_t uiId)
        {
            assert(mRegisteredUiItems.count(uiId));
            mRegisteredUiItems.erase(uiId);
        }

        void UiCanvas::RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            UnregisterUiItem(uiItem->GetId());
            const auto it = std::remove_if(mChildren.begin(), mChildren.end(), [&](const auto &childUi)
                                           { return childUi->GetId() == uiItem->GetId(); });
            mChildren.erase(it);
        }

        void UiCanvas::Render()
        {
            for (const auto &child : mChildren)
            {
                child->Render();
            }
        }
    }
}