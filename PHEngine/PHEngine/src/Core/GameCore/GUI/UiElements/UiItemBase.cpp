#include "UiItemBase.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/UiAnchorPositionHelper.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/LoggerExtension.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace EngineCore;
using namespace Graphics::Renderer;

namespace EngineCore
{
    namespace GUI
    {

        size_t UiItemBase::s_UIds = 0;

        UiItemBase::UiItemBase(const std::weak_ptr<UiCanvas> &parentCanvas, const std::weak_ptr<IUiTransformable> &parent)
            : mUId(s_UIds++),
              mName("UiItemBase_" + std::to_string(mUId)),
              mAbsoluteOrigin(),
              mNormalizedTranslation(),
              mNormalizedScale(glm::vec2(1.0)),
              mZOrder(0),
              mWidth(0),
              mHeight(0),
              mBoundingArea(),
              mAnchors(),
              mParent(parent),
              mParentCanvas(parentCanvas),
              mChildren(),
              mIsVisible(true),
              mIsTransformDirty(true),
              mIsPropertiesShouldBeUpdatedOnRenderThread(false)
        {
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

        const glm::ivec2 &UiItemBase::GetAbsoluteOrigin() const
        {
            return mAbsoluteOrigin;
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

        void UiItemBase::SetAbsoluteOrigin(const glm::ivec2 &transform)
        {
            if (!CheckSimilarityIVec2(mAbsoluteOrigin, transform))
            {
                mAbsoluteOrigin = transform;
                SetIsTransformDirty(true);
            }
        }

        void UiItemBase::SetZOrder(const size_t zOrder)
        {
            if (mZOrder != zOrder)
            {
                mZOrder = zOrder;
                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        void UiItemBase::SetWidth(const size_t width)
        {
            if (mWidth != width)
            {
                mWidth = width;
                SetIsTransformDirty(true);
            }
        }

        void UiItemBase::SetHeight(const size_t height)
        {
            if (mHeight != height)
            {
                mHeight = height;
                SetIsTransformDirty(true);
            }
        }

        bool UiItemBase::IsVisible() const
        {
            return mIsVisible;
        }

        void UiItemBase::SetIsVisible(const bool isVisible)
        {
            if (mIsVisible != isVisible)
            {
                mIsVisible = isVisible;
                SetChildrenIsVisible(mIsVisible);
                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        void UiItemBase::SetChildrenIsVisible(const bool isVisible)
        {
            for (const auto &child : mChildren)
            {
                child->SetIsVisible(isVisible);
            }
        }

        void UiItemBase::SetAnchor(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor, const std::string &dstUiItemName)
        {
            assert(UiAnchorPositionHelper::CheckIsAnchorBindingValid(srcAnchor, dstAnchor)); // Wrong anchor binding. srcAnchor
            assert(TryFindAncestryUiItem(dstUiItemName));                                    // Try to anchor to ui item which is not parent or sibling

            if (mAnchors.count(srcAnchor))
            {
                const auto &anchorData = mAnchors.at(srcAnchor);
                if (anchorData.GetDstAnchor() != dstAnchor && anchorData.GetDstUiItemName() != dstUiItemName)
                {
                    UiAnchorData anchorData;
                    anchorData.SetDstAnchor(dstAnchor);
                    anchorData.SetDstUiItemName(dstUiItemName);
                    mAnchors[srcAnchor] = anchorData;
                    SetIsTransformDirty(true);
                }
            }
            else
            {
                UiAnchorData anchorData;
                anchorData.SetDstAnchor(dstAnchor);
                anchorData.SetDstUiItemName(dstUiItemName);
                mAnchors[srcAnchor] = anchorData;
                SetIsTransformDirty(true);
            }
        }

        void UiItemBase::SetAnchorMargin(const eUiAnchor anchor, const int32_t anchorMargin)
        {
            assert(mAnchors.count(anchor) && mAnchors.at(anchor).GetDstAnchor() != eUiAnchor::NONE);
            if (mAnchors.at(anchor).GetSrcAnchorMargin() != anchorMargin)
            {
                mAnchors[anchor].SetSrcAnchorMargin(anchorMargin);
                SetIsTransformDirty(true);
            }
        }

        size_t UiItemBase::GetUId() const
        {
            return mUId;
        }

        std::string UiItemBase::GetName() const
        {
            return mName;
        }

        BoundingBox2D UiItemBase::GetBoundingArea() const
        {
            return mBoundingArea;
        }

        std::shared_ptr<IUiTransformable> UiItemBase::TryFindChildByName(const std::string &name) const
        {
            const auto foundIt = std::find_if(mChildren.begin(), mChildren.end(), [&name](const auto &child)
                                              { return child->GetName() == name; });
            return foundIt != mChildren.end() ? (*foundIt) : nullptr;
        }

        std::shared_ptr<IUiTransformable> UiItemBase::TryFindAncestryUiItem(const std::string &name) const
        {
            std::shared_ptr<IUiTransformable> result;
            auto parentWp = mParent;

            while (const auto &parentSp = parentWp.lock())
            {
                if (parentSp->GetName() == name)
                {
                    return parentSp;
                }
                else if (const auto &child = parentSp->TryFindChildByName(name))
                {
                    return child;
                }

                parentWp = parentSp->GetParent();
            }

            return result;
        }

        std::weak_ptr<IUiTransformable> UiItemBase::GetRootParent() const
        {
            const std::shared_ptr<IUiTransformable> &parent = mParent.lock();
            if (parent)
            {
                const auto &parentSp = parent->GetRootParent().lock();
                if (parentSp)
                {
                    return parentSp;
                }
            }
            return parent;
        }

        std::weak_ptr<IUiTransformable> UiItemBase::GetParent() const
        {
            return mParent;
        }

        std::shared_ptr<IUiMouseInputReceivable> UiItemBase::GetMouseInputReceiver() const
        {
            return mMouseInputReceiver;
        }

        bool UiItemBase::IsTransformDirty() const
        {
            return mIsTransformDirty;
        }

        void UiItemBase::SetIsTransformDirty(const bool isDirty)
        {
            mIsTransformDirty = isDirty;
        }

        void UiItemBase::SetIsPropertiesShouldBeUpdated(const bool update)
        {
            mIsPropertiesShouldBeUpdatedOnRenderThread = update;
        }

        void UiItemBase::SetMouseInputReceiver(const std::shared_ptr<IUiMouseInputReceivable> &inputReceiver)
        {
            if (const auto &canvasSp = mParentCanvas.lock())
            {
                assert(canvasSp->IsInputSystemInitialized());
                mMouseInputReceiver = inputReceiver;
            }
        }

        void UiItemBase::RebuildNormalizedTransform()
        {
            if (const auto &rootParentSp = GetRootParent().lock())
            {
                const auto rootWidth = rootParentSp->GetWidth();
                const auto rootHeight = rootParentSp->GetHeight();
                const auto rootOrigin = rootParentSp->GetAbsoluteOrigin();
                assert(rootWidth != 0 && rootHeight != 0);
                mNormalizedTranslation = glm::vec2((static_cast<float>(mAbsoluteOrigin.x) / static_cast<float>(rootWidth)) - (static_cast<float>(rootOrigin.x) / static_cast<float>(rootWidth)),
                                                   (static_cast<float>(mAbsoluteOrigin.y) / static_cast<float>(rootHeight)) - (static_cast<float>(rootOrigin.y) / static_cast<float>(rootHeight)));

                mNormalizedScale = glm::vec2(static_cast<float>(mWidth) / static_cast<float>(rootWidth),
                                             static_cast<float>(mHeight) / static_cast<float>(rootHeight));

                LogInfo("UiItemBase::RebuildNormalizedTransform => uid: ", mUId, " mAbsoluteOrigin: ", mAbsoluteOrigin,
                        " mWidth: ", mWidth, " mHeight: ", mHeight, " rootWidth: ", rootWidth, " rootHeight: ", rootHeight);

                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        void UiItemBase::RebuildBoundingArea()
        {
            const auto halfExtent = glm::ivec2(mWidth / 2, mHeight / 2);
            mBoundingArea = BoundingBox2D(mAbsoluteOrigin + halfExtent, halfExtent);
            LogInfo("UiItemBase::RebuildBoundingArea => uid: ", mUId, " bounding box: ", mBoundingArea);
        }

        void UiItemBase::RecalculateAnchorPositions()
        {
            CalculateHorizontalAnchorPositions();
            CalculateVerticalAnchorPositions();

            RebuildBoundingArea();
            RebuildNormalizedTransform();
        }

        void UiItemBase::CalculateHorizontalAnchorPositions()
        {
            if (mAnchors.count(eUiAnchor::HORIZONTAL_CENTER))
            {
                const auto &dstAnchor = mAnchors.at(eUiAnchor::HORIZONTAL_CENTER);
                const auto &dstAnchoringUiItem = TryFindAncestryUiItem(dstAnchor.GetDstUiItemName());
                assert(dstAnchoringUiItem);
                const auto &dstBoundingArea = dstAnchoringUiItem->GetBoundingArea();
                mAbsoluteOrigin.x = dstBoundingArea.GetOrigin().x - (mWidth / 2);
            }
            else
            {
                if (mAnchors.count(eUiAnchor::LEFT) && mAnchors.count(eUiAnchor::RIGHT))
                {
                    const auto &leftAnchor = mAnchors.at(eUiAnchor::LEFT);
                    const auto &rightAnchor = mAnchors.at(eUiAnchor::RIGHT);
                    std::shared_ptr<IUiTransformable> leftAnchorUiItem, rightAnchorUiItem;
                    if (leftAnchor.GetDstUiItemName() == rightAnchor.GetDstUiItemName())
                    {
                        leftAnchorUiItem = rightAnchorUiItem = TryFindAncestryUiItem(leftAnchor.GetDstUiItemName());
                    }
                    else
                    {
                        leftAnchorUiItem = TryFindAncestryUiItem(leftAnchor.GetDstUiItemName());
                        rightAnchorUiItem = TryFindAncestryUiItem(rightAnchor.GetDstUiItemName());
                    }

                    assert(leftAnchorUiItem && rightAnchorUiItem);
                    const auto &leftAnchorUiItemBoundingArea = leftAnchorUiItem->GetBoundingArea();
                    const auto &rightAnchorUiItemBoundingArea = rightAnchorUiItem->GetBoundingArea();

                    int32_t originX = 0, width = 0;

                    if (eUiAnchor::LEFT == leftAnchor.GetDstAnchor())
                    {
                        originX = leftAnchorUiItemBoundingArea.GetMin().x;
                    }
                    else if (eUiAnchor::RIGHT == leftAnchor.GetDstAnchor())
                    {
                        originX = leftAnchorUiItemBoundingArea.GetMax().x;
                    };

                    if (eUiAnchor::LEFT == rightAnchor.GetDstAnchor())
                    {
                        width = (rightAnchorUiItemBoundingArea.GetMin().x - originX) - (rightAnchor.GetSrcAnchorMargin() + leftAnchor.GetSrcAnchorMargin());
                    }
                    else if (eUiAnchor::RIGHT == rightAnchor.GetDstAnchor())
                    {
                        width = (rightAnchorUiItemBoundingArea.GetMax().x - originX) - (rightAnchor.GetSrcAnchorMargin() + leftAnchor.GetSrcAnchorMargin());
                    }

                    mAbsoluteOrigin.x = originX + leftAnchor.GetSrcAnchorMargin();
                    mWidth = width;
                }
                else if (mAnchors.count(eUiAnchor::LEFT))
                {
                    const auto &leftAnchor = mAnchors.at(eUiAnchor::LEFT);
                    const auto &leftAnchorUiItem = TryFindAncestryUiItem(leftAnchor.GetDstUiItemName());
                    assert(leftAnchorUiItem);
                    const auto &leftAnchorUiItemBoundingArea = leftAnchorUiItem->GetBoundingArea();

                    const int32_t originX = eUiAnchor::LEFT == leftAnchor.GetDstAnchor() ? leftAnchorUiItemBoundingArea.GetMin().x : eUiAnchor::RIGHT == leftAnchor.GetDstAnchor() ? leftAnchorUiItemBoundingArea.GetMax().x
                                                                                                                                                                                   : 0;
                    mAbsoluteOrigin.x = originX + leftAnchor.GetSrcAnchorMargin();
                }
                else if (mAnchors.count(eUiAnchor::RIGHT))
                {
                    const auto &rightAnchor = mAnchors.at(eUiAnchor::RIGHT);
                    const auto &rightAnchorUiItem = TryFindAncestryUiItem(rightAnchor.GetDstUiItemName());
                    assert(rightAnchorUiItem);

                    const auto &rightAnchorUiItemBoundingArea = rightAnchorUiItem->GetBoundingArea();

                    const int32_t anchorOriginX = eUiAnchor::LEFT == rightAnchor.GetDstAnchor() ? rightAnchorUiItemBoundingArea.GetMin().x : eUiAnchor::RIGHT == rightAnchor.GetDstAnchor() ? rightAnchorUiItemBoundingArea.GetMax().x
                                                                                                                                                                                            : 0;
                    mAbsoluteOrigin.x = anchorOriginX - mWidth - rightAnchor.GetSrcAnchorMargin();
                }
            }
            LogInfo("UiItemBase::CalculateHorizontalAnchorPositions => uid: ", mUId, " mAbsoluteOrigin: ", mAbsoluteOrigin, " mWidth: ", mWidth, " mHeight: ", mHeight);
        }

        void UiItemBase::CalculateVerticalAnchorPositions()
        {
            if (mAnchors.count(eUiAnchor::VERTICAL_CENTER))
            {
                const auto &dstAnchor = mAnchors.at(eUiAnchor::VERTICAL_CENTER);
                const auto &dstAnchoringUiItem = TryFindAncestryUiItem(dstAnchor.GetDstUiItemName());
                assert(dstAnchoringUiItem);
                const auto &dstBoundingArea = dstAnchoringUiItem->GetBoundingArea();
                mAbsoluteOrigin.y = dstBoundingArea.GetOrigin().y - (mHeight / 2);
            }
            else
            {
                if (mAnchors.count(eUiAnchor::BOTTOM) && mAnchors.count(eUiAnchor::TOP))
                {
                    const auto &bottomAnchor = mAnchors.at(eUiAnchor::BOTTOM);
                    const auto &topAnchor = mAnchors.at(eUiAnchor::TOP);
                    std::shared_ptr<IUiTransformable> bottomAnchorUiItem, topAnchorUiItem;
                    if (bottomAnchor.GetDstUiItemName() == topAnchor.GetDstUiItemName())
                    {
                        bottomAnchorUiItem = topAnchorUiItem = TryFindAncestryUiItem(bottomAnchor.GetDstUiItemName());
                    }
                    else
                    {
                        bottomAnchorUiItem = TryFindAncestryUiItem(bottomAnchor.GetDstUiItemName());
                        topAnchorUiItem = TryFindAncestryUiItem(topAnchor.GetDstUiItemName());
                    }

                    assert(bottomAnchorUiItem && topAnchorUiItem);
                    const auto &bottomAnchorUiItemBoundingArea = bottomAnchorUiItem->GetBoundingArea();
                    const auto &topAnchorUiItemBoundingArea = topAnchorUiItem->GetBoundingArea();

                    int32_t originY, height;
                    if (eUiAnchor::BOTTOM == bottomAnchor.GetDstAnchor())
                    {
                        originY = bottomAnchorUiItemBoundingArea.GetMin().y;
                    }
                    else if (eUiAnchor::TOP == bottomAnchor.GetDstAnchor())
                    {
                        originY = bottomAnchorUiItemBoundingArea.GetMax().y;
                    }

                    if (eUiAnchor::BOTTOM == topAnchor.GetDstAnchor())
                    {
                        height = (topAnchorUiItemBoundingArea.GetMin().y - originY) - (topAnchor.GetSrcAnchorMargin() + bottomAnchor.GetSrcAnchorMargin());
                    }
                    else if (eUiAnchor::TOP == topAnchor.GetDstAnchor())
                    {
                        height = (topAnchorUiItemBoundingArea.GetMax().y - originY) - (topAnchor.GetSrcAnchorMargin() + bottomAnchor.GetSrcAnchorMargin());
                    }

                    mAbsoluteOrigin.y = originY + bottomAnchor.GetSrcAnchorMargin();
                    mHeight = height;
                }
                else if (mAnchors.count(eUiAnchor::BOTTOM))
                {
                    const auto &bottomAnchor = mAnchors.at(eUiAnchor::BOTTOM);
                    const auto &bottomAnchorUiItem = TryFindAncestryUiItem(bottomAnchor.GetDstUiItemName());
                    assert(bottomAnchorUiItem);
                    const auto &bottomAnchorUiItemBoundingArea = bottomAnchorUiItem->GetBoundingArea();

                    const int32_t originY = eUiAnchor::BOTTOM == bottomAnchor.GetDstAnchor() ? bottomAnchorUiItemBoundingArea.GetMin().y : eUiAnchor::TOP == bottomAnchor.GetDstAnchor() ? bottomAnchorUiItemBoundingArea.GetMax().y
                                                                                                                                                                                         : 0;
                    mAbsoluteOrigin.y = originY + bottomAnchor.GetSrcAnchorMargin();
                }
                else if (mAnchors.count(eUiAnchor::TOP))
                {
                    const auto &topAnchor = mAnchors.at(eUiAnchor::TOP);
                    const auto &topAnchorUiItem = TryFindAncestryUiItem(topAnchor.GetDstUiItemName());
                    assert(topAnchorUiItem);

                    const auto &topAnchorUiItemBoundingArea = topAnchorUiItem->GetBoundingArea();

                    const int32_t anchorOriginY = eUiAnchor::BOTTOM == topAnchor.GetDstAnchor() ? topAnchorUiItemBoundingArea.GetMin().y : eUiAnchor::TOP == topAnchor.GetDstAnchor() ? topAnchorUiItemBoundingArea.GetMax().y
                                                                                                                                                                                      : 0;
                    mAbsoluteOrigin.y = anchorOriginY - mHeight - topAnchor.GetSrcAnchorMargin();
                }
            }
            LogInfo("UiItemBase::CalculateVerticalAnchorPositions => uid: ", mUId, " mAbsoluteOrigin: ", mAbsoluteOrigin, " mWidth: ", mWidth, " mHeight: ", mHeight);
        }

        void UiItemBase::AddUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            assert(GetName() == uiItem->GetParent().lock()->GetName());

            RegisterUiItem(uiItem->GetUId(), uiItem->GetName());
            mChildren.emplace_back(uiItem);
            uiItem->OnRegistered();

            if (const auto &canvasSp = mParentCanvas.lock())
            {
                canvasSp->CollectChildrenWithDescendingZOrder();
            }
        }

        void UiItemBase::RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            UnregisterUiItem(uiItem->GetUId(), uiItem->GetName());
            const auto it = std::remove_if(mChildren.begin(), mChildren.end(), [&](const auto &childUi)
                                           { return childUi->GetUId() == uiItem->GetUId(); });
            mChildren.erase(it);
            uiItem->OnUnregistered();

            if (const auto &canvasSp = mParentCanvas.lock())
            {
                canvasSp->CollectChildrenWithDescendingZOrder();
            }
        }

        void UiItemBase::RegisterUiItem(const size_t uiId, const std::string &uiItemName)
        {
            if (const auto &parentSp = mParent.lock())
            {
                parentSp->RegisterUiItem(uiId, uiItemName);
            }
        }

        void UiItemBase::UnregisterUiItem(const size_t uiId, const std::string &uiItemName)
        {
            if (const auto &parentSp = mParent.lock())
            {
                parentSp->UnregisterUiItem(uiId, uiItemName);
            }
        }

        void UiItemBase::OnRegistered()
        {
        }

        void UiItemBase::OnUnregistered()
        {
        }

        void UiItemBase::UnpausableTick(const float deltaTime)
        {
            if (mIsTransformDirty)
            {
                UpdateAnchorTransform();
                mIsTransformDirty = false;
            }

            if (mIsPropertiesShouldBeUpdatedOnRenderThread)
            {
                OnPropertiesShouldBeUpdatedOnRenderThread();
                mIsPropertiesShouldBeUpdatedOnRenderThread = false;
            }

            for (const auto &child : mChildren)
            {
                child->UnpausableTick(deltaTime);
            }
        }

        void UiItemBase::Tick(const float deltaTime)
        {
        }

        void UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread()
        {
            SyncDataOnRenderThread();
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

        void UiItemBase::UpdateAnchorTransform()
        {
            RecalculateAnchorPositions();

            if (const auto &canvasSp = mParentCanvas.lock())
            {
                const auto &dependentItems = canvasSp->GetDependentByTransformChildren(GetName());
                for (const auto &dependentItem : dependentItems)
                {
                    dependentItem->UpdateDependentChildrenAnchorTransform();
                }
            }
        }

        void UiItemBase::UpdateDependentChildrenAnchorTransform()
        {
            LogInfo("UiItemBase::UpdateDependentChildrenAnchorTransform => UiItem name: ", GetName());
            UpdateAnchorTransform();
        }

        bool UiItemBase::IsTransformDependentToUiItem(const std::string &uiItemName) const
        {
            return std::any_of(mAnchors.cbegin(), mAnchors.cend(), [&uiItemName](const auto &keyValueAnchor)
                               { return keyValueAnchor.second.GetDstUiItemName() == uiItemName; });
        }

        void UiItemBase::GetDependentByTransformChildren(const std::string &nameOfRelatedUiItem, std::vector<std::shared_ptr<UiItemBase>> &affectedUiItems)
        {
            LogInfo("UiItemBase::GetDependentByTransformChildren => UiItem name: ", GetName());
            for (const auto &child : mChildren)
            {
                if (child->IsTransformDependentToUiItem(nameOfRelatedUiItem))
                {
                    affectedUiItems.emplace_back(child);
                }

                child->GetDependentByTransformChildren(nameOfRelatedUiItem, affectedUiItems);
            }
        }

        void UiItemBase::CollectAllHierarchyChildren(std::vector<std::shared_ptr<UiItemBase>> &inCollection) const
        {
            for (const auto &child : mChildren)
            {
                inCollection.emplace_back(child);
                child->CollectAllHierarchyChildren(inCollection);
            }
        }

        void UiItemBase::OnMousePositionChanged(const glm::ivec2 &mouseCursorPosition)
        {
            if (mMouseInputReceiver)
            {
                mMouseInputReceiver->OnMousePositionChanged(GetBoundingArea(), mouseCursorPosition);
            }
        }

        void UiItemBase::OnMouseReleased(const glm::ivec2 &mouseCursorPosition)
        {
            if (mMouseInputReceiver)
            {
                mMouseInputReceiver->OnMouseReleased(GetBoundingArea(), mouseCursorPosition);
            }
        }

        void UiItemBase::OnMousePressed(const glm::ivec2 &mouseCursorPosition)
        {
            if (mMouseInputReceiver)
            {
                mMouseInputReceiver->OnMousePressed(GetBoundingArea(), mouseCursorPosition);
            }
        }

        void UiItemBase::OnMouseClicked(const glm::ivec2 &mouseCursorPosition)
        {
            if (mMouseInputReceiver)
            {
                mMouseInputReceiver->OnMouseClicked(GetBoundingArea(), mouseCursorPosition);
            }
        }

        void UiItemBase::SyncDataOnRenderThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiItemBase::SyncDataOnRenderThread");
            if (const auto &sceneSp = GetScene().lock())
            {
                if (const auto &canvasSp = GetParentCanvas().lock())
                {
                    if (const auto &sceneRenderer = sceneSp->GetThreadManager().TryGetSceneRendererWP().lock())
                    {
                        sceneSp->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [this, sceneRenderer, canvasSp]()
                                                       {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(GetUId(), canvasSp->GetUId());
                            if (uiSceneProxy)
                            {
                                uiSceneProxy->SetIsVisible(mIsVisible);
                                uiSceneProxy->SetZOrder(mZOrder);
                                uiSceneProxy->SetTransform(mNormalizedTranslation, mNormalizedScale);
                                LogInfo("UiItemBase::SyncDataOnRenderThread => name: ", GetName(), " translation = ", mNormalizedTranslation, " scale = ", mNormalizedScale);
                            } });
                    }
                }
            }
        }
    }
}