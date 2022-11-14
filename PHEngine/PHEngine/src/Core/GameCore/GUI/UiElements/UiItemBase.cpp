#include "UiItemBase.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace EngineCore
{
    namespace GUI
    {

        size_t UiItemBase::s_Ids = 0;

        UiItemBase::UiItemBase(const std::weak_ptr<UiItemBase> &parent)
            : mId(s_Ids++),
              mAbsoluteOrigin(),
              mRelativeOrigin(),
              mZOrder(0),
              mWidth(0),
              mHeight(0),
              mBoundingArea(),
              mAnchors(),
              mParent(parent),
              mChildren()
        {
        }

        const Transform2D &UiItemBase::GetAbsoluteOrigin() const
        {
            return mAbsoluteOrigin;
        }

        const Transform2D &UiItemBase::GetRelativeOrigin() const
        {
            return mRelativeOrigin;
        }

        size_t UiItemBase::GetZOrder() const
        {
            return mZOrder;
        }

        void UiItemBase::SetAbsoluteOrigin(const Transform2D &transform)
        {
            if (!CheckSimilarityIVec2(mAbsoluteOrigin.Translation, transform.Translation))
            {
                mAbsoluteOrigin = transform;
                TransformChanged();
            }
        }

        void UiItemBase::SetRelativeOrigin(const Transform2D &transform)
        {
            if (!CheckSimilarityIVec2(mRelativeOrigin.Translation, transform.Translation))
            {
                mRelativeOrigin = transform;
                TransformChanged();
            }
        }

        void UiItemBase::SetZOrder(const size_t zOrder)
        {
            if (mZOrder != zOrder)
            {
                mZOrder = zOrder;
                TransformChanged();
            }
        }

        void UiItemBase::SetWidth(const size_t width)
        {
            if (mWidth != width)
            {
                mWidth = width;
                TransformChanged();
            }
        }

        void UiItemBase::SetHeight(const size_t height)
        {
            if (mHeight != height)
            {
                mHeight = height;
                TransformChanged();
            }
        }

        size_t UiItemBase::GetId() const
        {
            return mId;
        }

        void UiItemBase::SetUiAnchor(const eUiAnchorType anchorType, const std::shared_ptr<UiItemBase> &anchorUiItem)
        {
            if (!mAnchors.count(anchorType) || mAnchors.at(anchorType)->GetId() != anchorUiItem->GetId())
            {
                mAnchors[anchorType] = anchorUiItem;
                TransformChanged();
            }
        }

        void UiItemBase::TransformChanged()
        {
            UpdateHierarchyTransform();
            OnTransformChanged();
        }

        void UiItemBase::RebuildTransform()
        {
            if (const auto parentSp = mParent.lock())
            {
                const auto parentWidth = parentSp->GetWidth();
                const auto parentHeight = parentSp->GetHeight();
                const auto parentAbsoluteOrigin = parentSp->GetAbsoluteOrigin();

                if (eUiItemPositioningType::ANCHORS == mUiPositioningType)
                {
                    if (mAnchors.size())
                    {
                        // calculate size and position with anchors
                    }
                }
                else if (eUiItemPositioningType::RELATIVE == mUiPositioningType)
                {
                    const auto relativeTranslation = glm::clamp(mRelativeOrigin.Translation, glm::ivec2(), glm::ivec2(parentWidth, parentHeight));
                    mAbsoluteOrigin.Translation = relativeTranslation + parentAbsoluteOrigin.Translation;
                }
            }
        }

        void UiItemBase::RebuildBoundingArea()
        {
            mBoundingArea = BoundingBox2D(mAbsoluteOrigin.Translation, glm::ivec2(mWidth / 2, mHeight / 2));
        }

        void UiItemBase::OnTransformChanged()
        {
        }

        void UiItemBase::UpdateHierarchyTransform()
        {
            RebuildTransform();
            RebuildBoundingArea();
            
            for (const auto &child : mChildren)
            {
                child->UpdateHierarchyTransform();
            }
        }
    }
}