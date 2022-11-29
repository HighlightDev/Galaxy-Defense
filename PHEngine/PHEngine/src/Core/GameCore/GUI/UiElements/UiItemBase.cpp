#include "UiItemBase.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace EngineMath;
using namespace EngineCore;

namespace EngineCore
{
    namespace GUI
    {

        size_t UiItemBase::s_UIds = 0;

        UiItemBase::UiItemBase(const std::weak_ptr<UiCanvas> &parentCanvas, const std::weak_ptr<IUiTransformable> &parent)
            : mUId(s_UIds++),
              mAbsoluteOrigin(),
              mRelativeOrigin(),
              mNormalizedTranslation(),
              mNormalizedScale(glm::vec2(1.0)),
              mTransformMatrix(glm::mat4(1)),
              mZOrder(0),
              mWidth(0),
              mHeight(0),
              mBoundingArea(),
              mAnchors(),
              mParent(parent),
              mParentCanvas(parentCanvas),
              mChildren(),
              mIsVisible(true)
        {
            TransformChanged();
        }

        std::weak_ptr<Scene> UiItemBase::GetScene() const
        {
            if (const auto &parentSp = mParent.lock())
            {
                return parentSp->GetScene();
            }

            return std::weak_ptr<Scene>();
        }

        const std::weak_ptr<UiCanvas> &UiItemBase::GetParentCanvas() const
        {
            return mParentCanvas;
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
            return mTransformMatrix;
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

        size_t UiItemBase::GetUId() const
        {
            return mUId;
        }

        void UiItemBase::SetUiAnchor(const eUiAnchorType anchorType, const std::shared_ptr<UiItemBase> &anchorUiItem)
        {
            if (!mAnchors.count(anchorType) || mAnchors.at(anchorType)->GetUId() != anchorUiItem->GetUId())
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
                RebuildTransformMatrix();
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

        void UiItemBase::RebuildTransformMatrix()
        {
            mTransformMatrix = glm::mat4(1);
            const auto &tranlsationToLeftBottomCorner = (glm::vec2(1.0f) - mNormalizedScale);
            const auto translationFromCenter = mNormalizedTranslation - tranlsationToLeftBottomCorner;
            mTransformMatrix *= glm::translate(glm::mat4(1), glm::vec3(translationFromCenter.x, translationFromCenter.y, 0.0f));
            mTransformMatrix *= glm::scale(glm::mat4(1), glm::vec3(mNormalizedScale.x, mNormalizedScale.y, 1.0f));
        }

        void UiItemBase::RebuildBoundingArea()
        {
            mBoundingArea = BoundingBox2D(mAbsoluteOrigin.Translation, glm::ivec2(mWidth / 2, mHeight / 2));
        }

        void UiItemBase::OnTransformChanged()
        {
        }

        bool UiItemBase::IsVisible() const
        {
            return mIsVisible;
        }

        void UiItemBase::SetIsVisible(const bool isVisible)
        {
            mIsVisible = isVisible;
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
            RegisterUiItem(uiItem->GetUId());
            mChildren.emplace_back(uiItem);
            uiItem->OnRegistered();
        }

        void UiItemBase::RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            UnregisterUiItem(uiItem->GetUId());
            const auto it = std::remove_if(mChildren.begin(), mChildren.end(), [&](const auto &childUi)
                                           { return childUi->GetUId() == uiItem->GetUId(); });
            mChildren.erase(it);
            uiItem->OnDeregistered();
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

        void UiItemBase::OnRegistered()
        {
        }

        void UiItemBase::OnDeregistered()
        {
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
    }
}