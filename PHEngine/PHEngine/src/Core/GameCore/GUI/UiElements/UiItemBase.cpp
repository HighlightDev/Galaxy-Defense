#include "UiItemBase.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace EngineMath;

namespace EngineCore
{
    namespace GUI
    {

        size_t UiItemBase::s_Ids = 0;

        UiItemBase::UiItemBase(const std::weak_ptr<IUiTransformable> &parent)
            : mId(s_Ids++),
              mAbsoluteOrigin(),
              mRelativeOrigin(),
              mNormalizedTranslation(),
              mNormalizedScale(glm::vec2(1.0)),
              mZOrder(0),
              mWidth(0),
              mHeight(0),
              mBoundingArea(),
              mAnchors(),
              mParent(parent),
              mChildren()
        {
            TransformChanged();
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

        size_t UiItemBase::GetWidth() const
        {
            return mWidth;
        }

        size_t UiItemBase::GetHeight() const
        {
            return mHeight;
        }

        glm::vec2 UiItemBase::GetNormalizedTranslation() const
        {
            return mNormalizedTranslation;
        }

        glm::vec2 UiItemBase::GetNormalizedScale() const
        {
            return mNormalizedScale;
        }

        glm::mat4 UiItemBase::GetTransformMatrix() const
        {
            glm::mat4 transformMatrix(1);
            const auto &tranlsationToLeftBottomCorner = glm::vec2(1.0f) - (mNormalizedScale * glm::vec2(0.5f));
            transformMatrix = glm::scale(transformMatrix, glm::vec3(mNormalizedScale.x, mNormalizedScale.y, 1.0f));
            transformMatrix = glm::translate(transformMatrix, glm::vec3(mNormalizedTranslation.x - tranlsationToLeftBottomCorner.x,
                                                                        mNormalizedTranslation.y - tranlsationToLeftBottomCorner.y, 0.0f));
            return transformMatrix;
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
                UpdateSortedChildren();
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

        std::shared_ptr<IUiTransformable> UiItemBase::GetRootParent() const
        {
            const std::shared_ptr<IUiTransformable> &parent = mParent.lock();
            if (parent)
            {
                const auto &parentSp = parent->GetRootParent();
                if (parentSp)
                {
                    return parentSp;
                }
            }
            return parent;
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
                        assert(false);
                        // todo: not implemented
                        // calculate size and position with anchors
                    }
                }
                else if (eUiItemPositioningType::RELATIVE == mUiPositioningType)
                {
                    const auto relativeTranslation = glm::clamp(mRelativeOrigin.Translation, glm::ivec2(), glm::ivec2(parentWidth, parentHeight));
                    mAbsoluteOrigin.Translation = relativeTranslation + parentAbsoluteOrigin.Translation;
                }

                RebuildNormalizedTransform(parentSp);
            }
        }

        void UiItemBase::RebuildNormalizedTransform(const std::shared_ptr<IUiTransformable> &parent)
        {
            auto rootParentSp = parent->GetRootParent();
            rootParentSp = rootParentSp ? rootParentSp : parent;
            const auto rootWidth = rootParentSp->GetWidth();
            const auto rootHeight = rootParentSp->GetHeight();
            assert(rootWidth != 0 && rootHeight != 0);
            mNormalizedTranslation = glm::vec2(static_cast<float>(mAbsoluteOrigin.Translation.x) / static_cast<float>(rootWidth),
                                               static_cast<float>(mAbsoluteOrigin.Translation.y) / static_cast<float>(rootHeight));

            mNormalizedScale = glm::vec2(static_cast<float>(mWidth) / static_cast<float>(rootWidth),
                                         static_cast<float>(mHeight) / static_cast<float>(rootHeight));
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

        void UiItemBase::AddUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            RegisterUiItem(uiItem->GetId());
            mChildren.emplace_back(uiItem);
            UpdateSortedChildren();
        }

        void UiItemBase::RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            UnregisterUiItem(uiItem->GetId());
            const auto it = std::remove_if(mChildren.begin(), mChildren.end(), [&](const auto &childUi)
                                           { return childUi->GetId() == uiItem->GetId(); });
            mChildren.erase(it);
            UpdateSortedChildren();
        }

        void UiItemBase::RegisterUiItem(const size_t uiId)
        {
            if (const auto &parentSp = mParent.lock())
            {
                parentSp->RegisterUiItem(uiId);
            }
        }

        void UiItemBase::UnregisterUiItem(const size_t uiId)
        {
            if (const auto &parentSp = mParent.lock())
            {
                parentSp->UnregisterUiItem(uiId);
            }
        }

        std::vector<std::shared_ptr<UiItemBase>> UiItemBase::GetAllChildren() const
        {
            using returnValue_t = decltype(GetAllChildren());
            returnValue_t result;

            result.insert(result.end(), mChildren.begin(), mChildren.end());

            for (const auto &child : mChildren)
            {
                const auto grandChildren = child->GetAllChildren();
                result.insert(result.end(), grandChildren.begin(), grandChildren.end());
            }

            return result;
        }

        void UiItemBase::UpdateSortedChildren()
        {
            if (const auto &parentSp = mParent.lock())
            {
                parentSp->UpdateSortedChildren();
            }
        }
    }
}