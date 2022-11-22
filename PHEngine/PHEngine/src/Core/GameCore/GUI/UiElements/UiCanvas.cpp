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
              mRegisteredUiItems(),
              mSortedChildren()
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

        glm::vec2 UiCanvas::GetNormalizedTranslation() const
        {
            return glm::vec2();
        }

        glm::vec2 UiCanvas::GetNormalizedScale() const
        {
            return glm::vec2(1.0);
        }

        std::shared_ptr<IUiTransformable> UiCanvas::GetRootParent() const
        {
            return std::shared_ptr<IUiTransformable>();
        }

        void UiCanvas::SetAbsoluteOrigin(const Transform2D &transform)
        {
            mAbsoluteOrigin = transform;
            UpdateHierarchyTransform();
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
            UpdateHierarchyTransform();
        }

        void UiCanvas::SetHeight(const size_t height)
        {
            mWidthHeight.y = height;
            UpdateHierarchyTransform();
        }

        void UiCanvas::UpdateHierarchyTransform()
        {
            for (const auto &child : mChildren)
            {
                child->UpdateHierarchyTransform();
            }
        }

        void UiCanvas::AddUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            RegisterUiItem(uiItem->GetId());
            mChildren.emplace_back(uiItem);
            UpdateSortedChildren();
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
            UpdateSortedChildren();
        }

        void UiCanvas::Render()
        {
            for (const auto &child : mSortedChildren)
            {
                child->Render();
            }
        }

        void UiCanvas::SortChildrenByZOrder()
        {
            mSortedChildren.clear();
            mSortedChildren.insert(mSortedChildren.end(), mChildren.begin(), mChildren.end());

            for (const auto &child : mChildren)
            {
                const auto grandChildren = child->GetAllChildren();
                mSortedChildren.insert(mSortedChildren.end(), grandChildren.begin(), grandChildren.end());
            }

            std::sort(mSortedChildren.begin(), mSortedChildren.end(), [](const auto &left, const auto &right)
                      { return left->GetZOrder() < right->GetZOrder(); });
        }

        void UiCanvas::UpdateSortedChildren()
        {
            SortChildrenByZOrder();
        }
    }
}