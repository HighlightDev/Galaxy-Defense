#include "UiCanvas.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/UiSceneProxy/UiCanvasSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/UtilityCore/EngineMath.h"

#include <algorithm>

using namespace EngineCore;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;

namespace EngineCore
{
    namespace GUI
    {
        size_t UiCanvas::s_UId = 0;

        UiCanvas::UiCanvas(const ViewPortInfo &canvasScreenProperties)
            : mUId(s_UId++),
              mName("UiCanvas_" + std::to_string(mUId)),
              mAbsoluteOrigin(glm::ivec2(canvasScreenProperties.OriginX, canvasScreenProperties.OriginY)),
              mWidthHeight(glm::ivec2(canvasScreenProperties.Width, canvasScreenProperties.Height)),
              mChildren(),
              mRegisteredUIds(),
              mRegisteredNames(),
              mIsVisible(true),
              mIsTransformDirty(true),
              mInputSystem(),
              mDescendingByZOrderHierarchyChildren()
        {
        }

        void UiCanvas::InitializeInputSystem()
        {
            if (!mInputSystem)
            {
                mInputSystem = std::make_unique<UiInputSystem>(shared_from_this());
            }
        }
        void UiCanvas::SetScene(const std::weak_ptr<Scene> &sceneWp)
        {
            mScene = sceneWp;
        }

        size_t UiCanvas::GetUId() const
        {
            return mUId;
        }

        bool UiCanvas::IsTransformDirty() const
        {
            return mIsTransformDirty;
        }

        std::weak_ptr<Scene> UiCanvas::GetScene() const
        {
            return mScene;
        }

        const glm::ivec2 &UiCanvas::GetAbsoluteOrigin() const
        {
            return mAbsoluteOrigin;
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
            return glm::vec2(static_cast<float>(mAbsoluteOrigin.x) / static_cast<float>(mWidthHeight.x),
                             static_cast<float>(mAbsoluteOrigin.y) / static_cast<float>(mWidthHeight.y));
        }

        glm::vec2 UiCanvas::GetNormalizedScale() const
        {
            return glm::vec2(1.0);
        }

        std::weak_ptr<IUiTransformable> UiCanvas::GetRootParent() const
        {
            return std::weak_ptr<IUiTransformable>();
        }

        std::weak_ptr<IUiTransformable> UiCanvas::GetParent() const
        {
            return std::weak_ptr<IUiTransformable>();
        }

        bool UiCanvas::IsVisible() const
        {
            return mIsVisible;
        }

        void UiCanvas::SetIsTransformDirty(const bool isDirty)
        {
            mIsTransformDirty = isDirty;
        }

        void UiCanvas::SetIsVisible(const bool isVisible)
        {
            if (mIsVisible != isVisible)
            {
                mIsVisible = isVisible;
                mIsPropertiesShouldBeUpdatedOnRenderThread = true;
            }
        }

        void UiCanvas::SetAbsoluteOrigin(const glm::ivec2 &transform)
        {
            if (!EngineMath::CheckSimilarityIVec2(transform, mAbsoluteOrigin))
            {
                mAbsoluteOrigin = transform;
                SetIsTransformDirty(true);
            }
        }

        void UiCanvas::SetZOrder(const size_t z_order)
        {
        }

        void UiCanvas::SetWidth(const size_t width)
        {
            if (mWidthHeight.x != width)
            {
                mWidthHeight.x = width;
                SetIsTransformDirty(true);
            }
        }

        void UiCanvas::SetHeight(const size_t height)
        {
            if (mWidthHeight.y != height)
            {
                mWidthHeight.y = height;
                SetIsTransformDirty(true);
            }
        }

        std::string UiCanvas::GetName() const
        {
            return mName;
        }

        BoundingBox2D UiCanvas::GetBoundingArea() const
        {
            const auto &halfExtent = mWidthHeight / 2;
            return BoundingBox2D(mAbsoluteOrigin + halfExtent, halfExtent);
        }

        void UiCanvas::SetAnchor(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor, const std::string &dstUiItemName)
        {
        }

        void UiCanvas::SetAnchorMargin(const eUiAnchor anchor, const int32_t anchorMargin)
        {
        }

        void UiCanvas::AddUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            RegisterUiItem(uiItem->GetUId(), uiItem->GetName());
            mChildren.emplace_back(uiItem);
            uiItem->OnRegistered();
            CollectChildrenWithDescendingZOrder();
        }

        void UiCanvas::RegisterUiItem(const size_t uiId, const std::string &uiItemName)
        {
            assert(!mRegisteredUIds.count(uiId) && !mRegisteredNames.count(uiItemName));
            mRegisteredUIds.insert(uiId);
            mRegisteredNames.insert(uiItemName);
        }

        void UiCanvas::UnregisterUiItem(const size_t uiId, const std::string &uiItemName)
        {
            assert(mRegisteredUIds.count(uiId) && mRegisteredNames.count(uiItemName));
            mRegisteredUIds.erase(uiId);
            mRegisteredNames.erase(uiItemName);
            CollectChildrenWithDescendingZOrder();
        }

        void UiCanvas::RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            UnregisterUiItem(uiItem->GetUId(), uiItem->GetName());
            const auto foundIt = std::find_if(mChildren.begin(), mChildren.end(), [&](const auto &childItem)
                                              { return uiItem->GetUId() == childItem->GetUId(); });
            (*foundIt)->OnUnregistered();
            const auto it = std::remove_if(mChildren.begin(), mChildren.end(), [&](const auto &childUi)
                                           { return childUi->GetUId() == uiItem->GetUId(); });
            mChildren.erase(it);
        }

        void UiCanvas::UnpausableTick(const float deltaTime)
        {
            if (mIsTransformDirty)
            {
                UpdateAnchorTransform();
                UpdateDependentChildrenAnchorTransform();
                mIsTransformDirty = false;
            }

            if (mIsPropertiesShouldBeUpdatedOnRenderThread)
            {
                SyncDataOnRenderThread();
                mIsPropertiesShouldBeUpdatedOnRenderThread = false;
            }

            for (const auto &child : mChildren)
            {
                child->UnpausableTick(deltaTime);
            }

            if (mInputSystem && mIsVisible)
            {
                mInputSystem->UnpausableTick(deltaTime);
            }
        }

        void UiCanvas::Tick(const float deltaTime)
        {
        }

        std::shared_ptr<UiCanvasSceneProxy> UiCanvas::CreateUiCanvasSceneProxy() const
        {
            return std::make_shared<UiCanvasSceneProxy>(this);
        }

        std::shared_ptr<IUiTransformable> UiCanvas::TryFindChildByName(const std::string &name) const
        {
            const auto foundIt = std::find_if(mChildren.begin(), mChildren.end(), [&name](const auto &child)
                                              { return child->GetName() == name; });
            return foundIt != mChildren.end() ? (*foundIt) : nullptr;
        }

        std::vector<std::shared_ptr<UiItemBase>> UiCanvas::GetDependentByTransformChildren(const std::string &nameOfChangedTransformUiItem) const
        {
            std::vector<std::shared_ptr<UiItemBase>> result;

            for (const auto &child : mChildren)
            {
                if (child->IsTransformDependentToUiItem(nameOfChangedTransformUiItem))
                {
                    result.emplace_back(child);
                }

                child->GetDependentByTransformChildren(nameOfChangedTransformUiItem, result);
            }

            return result;
        }

        void UiCanvas::UpdateAnchorTransform()
        {
            mIsPropertiesShouldBeUpdatedOnRenderThread = true;
        }

        void UiCanvas::UpdateDependentChildrenAnchorTransform()
        {
            const auto &dependentUiItems = GetDependentByTransformChildren(GetName());

            for (const auto &dependentItem : dependentUiItems)
            {
                dependentItem->UpdateDependentChildrenAnchorTransform();
            }
        }

        void UiCanvas::SyncDataOnRenderThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiCanvas::SyncDataOnRenderThread");
            if (const auto &sceneSp = mScene.lock())
            {
                if (const auto &sceneRenderer = sceneSp->GetThreadManager().TryGetSceneRendererWP().lock())
                {
                    sceneSp->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, 0, functionId, [this, sceneRenderer]()
                                                   {
                        const auto& canvasProxy = sceneRenderer->GetCanvasSceneProxyByProxyId(GetUId());
                        canvasProxy->SetIsVisible(mIsVisible),
                        canvasProxy->SetAbsoluteOrigin(mAbsoluteOrigin),
                        canvasProxy->SetWidthHeight(mWidthHeight); });
                }
            }
        }

        void UiCanvas::CollectChildrenWithDescendingZOrder()
        {
            std::vector<std::shared_ptr<UiItemBase>> childrenWithDescendingOrder;
            for (const auto &child : mChildren)
            {
                childrenWithDescendingOrder.emplace_back(child);
                child->CollectAllHierarchyChildren(childrenWithDescendingOrder);
            }

            std::sort(childrenWithDescendingOrder.begin(), childrenWithDescendingOrder.end(),
                      [](const auto &left, const auto &right)
                      { return left->GetZOrder() < right->GetZOrder(); });

            mDescendingByZOrderHierarchyChildren.clear();

            std::transform(childrenWithDescendingOrder.begin(), childrenWithDescendingOrder.end(),
                           std::back_inserter(mDescendingByZOrderHierarchyChildren),
                           [](const auto &childSp)
                           { return std::weak_ptr<UiItemBase>(childSp); });
        }

        void UiCanvas::OnMousePositionChanged(const glm::ivec2 &mouseCursorPosition)
        {
            const auto &boundingArea = GetBoundingArea();
            if (EngineMath::TestPointInAABB(boundingArea.GetMin(), boundingArea.GetMax(), mouseCursorPosition))
            {
                mWasHoveredLastFrame = true;
                for (const auto &childWp : mDescendingByZOrderHierarchyChildren)
                {
                    if (const auto &childSp = childWp.lock())
                    {
                        childSp->OnMousePositionChanged(mouseCursorPosition);
                    }
                }
            }
            else if (mWasHoveredLastFrame)
            {
                mWasHoveredLastFrame = false;
                for (const auto &childWp : mDescendingByZOrderHierarchyChildren)
                {
                    if (const auto &childSp = childWp.lock())
                    {
                        childSp->OnMousePositionChanged(mouseCursorPosition);
                    }
                }
            }
        }

        void UiCanvas::OnMouseReleased(const glm::ivec2 &mouseCursorPosition)
        {
            if (mMouseButtonWasPressedLastFrame)
            {
                for (const auto &childWp : mDescendingByZOrderHierarchyChildren)
                {
                    if (const auto &childSp = childWp.lock())
                    {
                        childSp->OnMouseReleased(mouseCursorPosition);
                    }
                }
            }

            mMouseButtonWasPressedLastFrame = false;
        }

        void UiCanvas::OnMousePressed(const glm::ivec2 &mouseCursorPosition)
        {
            const auto &boundingArea = GetBoundingArea();
            if (EngineMath::TestPointInAABB(boundingArea.GetMin(), boundingArea.GetMax(), mouseCursorPosition))
            {
                mMouseButtonWasPressedLastFrame = true;
                for (const auto &childWp : mDescendingByZOrderHierarchyChildren)
                {
                    if (const auto &childSp = childWp.lock())
                    {
                        childSp->OnMousePressed(mouseCursorPosition);
                    }
                }
            }
        }

        void UiCanvas::OnMouseClicked(const glm::ivec2 &mouseCursorPosition)
        {
            const auto &boundingArea = GetBoundingArea();
            if (EngineMath::TestPointInAABB(boundingArea.GetMin(), boundingArea.GetMax(), mouseCursorPosition))
            {
                for (const auto &childWp : mDescendingByZOrderHierarchyChildren)
                {
                    if (const auto &childSp = childWp.lock())
                    {
                        childSp->OnMouseClicked(mouseCursorPosition);
                    }
                }
            }
        }
    }
}