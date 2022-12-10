#include "UiItemBase.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/UiAnchorPositionHelper.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/LoggerExtension.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace EngineMath;
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
              mIsTransformDirty(true)
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
                SyncDataOnRenderThread();
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
                SyncDataOnRenderThread();
            }
        }

        void UiItemBase::SetAnchor(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor, const std::string &dstUiItemName)
        {
            assert(UiAnchorPositionHelper::CheckIsAnchorBindingValid(srcAnchor, dstAnchor)); // Wrong anchor binding. srcAnchor
            assert(TryFindAncestryUiItem(dstUiItemName));                                    // Try to anchor to ui item which is not parent or sibling

            if (mAnchors.count(srcAnchor))
            {
                const auto &anchorTargetPair = mAnchors.at(srcAnchor);
                if (anchorTargetPair.first != dstAnchor && anchorTargetPair.second != dstUiItemName)
                {
                    mAnchors[srcAnchor] = std::make_pair(dstAnchor, dstUiItemName);
                    SetIsTransformDirty(true);
                }
            }
            else
            {
                mAnchors[srcAnchor] = std::make_pair(dstAnchor, dstUiItemName);
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
                const auto &child = parentSp->TryFindChildByName(name);
                if (child)
                {
                    result = child;
                    break;
                }
                else
                {
                    result = parentSp;
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

        bool UiItemBase::IsTransformDirty() const
        {
            return mIsTransformDirty;
        }

        void UiItemBase::SetIsTransformDirty(const bool isDirty)
        {
            mIsTransformDirty = isDirty;
        }

        void UiItemBase::RebuildNormalizedTransform()
        {
            if (const auto &rootParentSp = GetRootParent().lock())
            {
                const auto rootWidth = rootParentSp->GetWidth();
                const auto rootHeight = rootParentSp->GetHeight();
                assert(rootWidth != 0 && rootHeight != 0);
                mNormalizedTranslation = glm::vec2(static_cast<float>(mAbsoluteOrigin.x) / static_cast<float>(rootWidth),
                                                   static_cast<float>(mAbsoluteOrigin.y) / static_cast<float>(rootHeight));

                mNormalizedScale = glm::vec2(static_cast<float>(mWidth) / static_cast<float>(rootWidth),
                                             static_cast<float>(mHeight) / static_cast<float>(rootHeight));

                LogInfo("UiItemBase::RebuildNormalizedTransform => uid: ", mUId, " mAbsoluteOrigin: ", mAbsoluteOrigin,
                        " mWidth: ", mWidth, " mHeight: ", mHeight, " rootWidth: ", rootWidth, " rootHeight: ", rootHeight);

                SyncDataOnRenderThread();
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
                const auto &dstAnchoringUiItem = TryFindAncestryUiItem(dstAnchor.second);
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
                    if (leftAnchor.second == rightAnchor.second)
                    {
                        leftAnchorUiItem = rightAnchorUiItem = TryFindAncestryUiItem(leftAnchor.second);
                    }
                    else
                    {
                        leftAnchorUiItem = TryFindAncestryUiItem(leftAnchor.second);
                        rightAnchorUiItem = TryFindAncestryUiItem(rightAnchor.second);
                    }

                    assert(leftAnchorUiItem && rightAnchorUiItem);
                    const auto &leftAnchorUiItemBoundingArea = leftAnchorUiItem->GetBoundingArea();
                    const auto &rightAnchorUiItemBoundingArea = rightAnchorUiItem->GetBoundingArea();

                    const int32_t originX = eUiAnchor::LEFT == leftAnchor.first ? leftAnchorUiItemBoundingArea.GetMin().x : eUiAnchor::RIGHT == leftAnchor.first ? leftAnchorUiItemBoundingArea.GetMax().x
                                                                                                                                                                 : 0;
                    const int32_t width = eUiAnchor::LEFT == rightAnchor.first ? rightAnchorUiItemBoundingArea.GetMin().x : eUiAnchor::RIGHT == rightAnchor.first ? rightAnchorUiItemBoundingArea.GetMax().x
                                                                                                                                                                  : 0;

                    mAbsoluteOrigin.x = originX;
                    mWidth = width;
                }
                else if (mAnchors.count(eUiAnchor::LEFT))
                {
                    const auto &leftAnchor = mAnchors.at(eUiAnchor::LEFT);
                    const auto &leftAnchorUiItem = TryFindAncestryUiItem(leftAnchor.second);
                    assert(leftAnchorUiItem);
                    const auto &leftAnchorUiItemBoundingArea = leftAnchorUiItem->GetBoundingArea();

                    const int32_t originX = eUiAnchor::LEFT == leftAnchor.first ? leftAnchorUiItemBoundingArea.GetMin().x : eUiAnchor::RIGHT == leftAnchor.first ? leftAnchorUiItemBoundingArea.GetMax().x
                                                                                                                                                                 : 0;
                    mAbsoluteOrigin.x = originX;
                }
                else if (mAnchors.count(eUiAnchor::RIGHT))
                {
                    const auto &rightAnchor = mAnchors.at(eUiAnchor::RIGHT);
                    const auto &rightAnchorUiItem = TryFindAncestryUiItem(rightAnchor.second);
                    assert(rightAnchorUiItem);

                    const auto &rightAnchorUiItemBoundingArea = rightAnchorUiItem->GetBoundingArea();

                    const int32_t anchorOriginX = eUiAnchor::LEFT == rightAnchor.first ? rightAnchorUiItemBoundingArea.GetMin().x : eUiAnchor::RIGHT == rightAnchor.first ? rightAnchorUiItemBoundingArea.GetMax().x
                                                                                                                                                                          : 0;
                    mAbsoluteOrigin.x = anchorOriginX - mWidth;
                }
            }
            LogInfo("UiItemBase::CalculateHorizontalAnchorPositions => uid: ", mUId, " mAbsoluteOrigin: ", mAbsoluteOrigin, " mWidth: ", mWidth, " mHeight: ", mHeight);
        }

        void UiItemBase::CalculateVerticalAnchorPositions()
        {
            if (mAnchors.count(eUiAnchor::VERTICAL_CENTER))
            {
                const auto &dstAnchor = mAnchors.at(eUiAnchor::VERTICAL_CENTER);
                const auto &dstAnchoringUiItem = TryFindAncestryUiItem(dstAnchor.second);
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
                    if (bottomAnchor.second == topAnchor.second)
                    {
                        bottomAnchorUiItem = topAnchorUiItem = TryFindAncestryUiItem(bottomAnchor.second);
                    }
                    else
                    {
                        bottomAnchorUiItem = TryFindAncestryUiItem(bottomAnchor.second);
                        topAnchorUiItem = TryFindAncestryUiItem(topAnchor.second);
                    }

                    assert(bottomAnchorUiItem && topAnchorUiItem);
                    const auto &bottomAnchorUiItemBoundingArea = bottomAnchorUiItem->GetBoundingArea();
                    const auto &topAnchorUiItemBoundingArea = topAnchorUiItem->GetBoundingArea();

                    const int32_t originY = eUiAnchor::BOTTOM == bottomAnchor.first ? bottomAnchorUiItemBoundingArea.GetMin().y : eUiAnchor::TOP == bottomAnchor.first ? bottomAnchorUiItemBoundingArea.GetMax().y
                                                                                                                                                                       : 0;
                    const int32_t height = eUiAnchor::BOTTOM == topAnchor.first ? topAnchorUiItemBoundingArea.GetMin().y : eUiAnchor::TOP == topAnchor.first ? topAnchorUiItemBoundingArea.GetMax().y
                                                                                                                                                             : 0;

                    mAbsoluteOrigin.y = originY;
                    mHeight = height;
                }
                else if (mAnchors.count(eUiAnchor::BOTTOM))
                {
                    const auto &bottomAnchor = mAnchors.at(eUiAnchor::BOTTOM);
                    const auto &bottomAnchorUiItem = TryFindAncestryUiItem(bottomAnchor.second);
                    assert(bottomAnchorUiItem);
                    const auto &bottomAnchorUiItemBoundingArea = bottomAnchorUiItem->GetBoundingArea();

                    const int32_t originY = eUiAnchor::BOTTOM == bottomAnchor.first ? bottomAnchorUiItemBoundingArea.GetMin().x : eUiAnchor::TOP == bottomAnchor.first ? bottomAnchorUiItemBoundingArea.GetMax().x
                                                                                                                                                                       : 0;
                    mAbsoluteOrigin.y = originY;
                }
                else if (mAnchors.count(eUiAnchor::TOP))
                {
                    const auto &topAnchor = mAnchors.at(eUiAnchor::TOP);
                    const auto &topAnchorUiItem = TryFindAncestryUiItem(topAnchor.second);
                    assert(topAnchorUiItem);

                    const auto &topAnchorUiItemBoundingArea = topAnchorUiItem->GetBoundingArea();

                    const int32_t anchorOriginY = eUiAnchor::BOTTOM == topAnchor.first ? topAnchorUiItemBoundingArea.GetMin().x : eUiAnchor::TOP == topAnchor.first ? topAnchorUiItemBoundingArea.GetMax().x
                                                                                                                                                                    : 0;
                    mAbsoluteOrigin.y = anchorOriginY - mHeight;
                }
            }
            LogInfo("UiItemBase::CalculateVerticalAnchorPositions => uid: ", mUId, " mAbsoluteOrigin: ", mAbsoluteOrigin, " mWidth: ", mWidth, " mHeight: ", mHeight);
        }

        void UiItemBase::AddUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            RegisterUiItem(uiItem->GetUId(), uiItem->GetName());
            mChildren.emplace_back(uiItem);
            uiItem->OnRegistered();
        }

        void UiItemBase::RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            UnregisterUiItem(uiItem->GetUId(), uiItem->GetName());
            const auto it = std::remove_if(mChildren.begin(), mChildren.end(), [&](const auto &childUi)
                                           { return childUi->GetUId() == uiItem->GetUId(); });
            mChildren.erase(it);
            uiItem->OnUnregistered();
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

        void UiItemBase::Tick(const float deltaTime)
        {
            if (mIsTransformDirty)
            {
                UpdateAnchorTransform();
                mIsTransformDirty = false;
            }

            for (const auto &child : mChildren)
            {
                child->Tick(deltaTime);
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
            UpdateAnchorTransform();
        }

        bool UiItemBase::IsTransformDependentToUiItem(const std::string &uiItemName) const
        {
            return std::any_of(mAnchors.cbegin(), mAnchors.cend(), [&uiItemName](const auto &keyValueAnchor)
                               { return keyValueAnchor.second.second == uiItemName; });
        }

        void UiItemBase::GetDependentByTransformChildren(const std::string &nameOfRelatedUiItem, std::vector<std::shared_ptr<UiItemBase>> &affectedUiItems)
        {
            for (const auto &child : mChildren)
            {
                if (child->IsTransformDependentToUiItem(nameOfRelatedUiItem))
                {
                    affectedUiItems.emplace_back(child);
                }

                child->GetDependentByTransformChildren(nameOfRelatedUiItem, affectedUiItems);
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