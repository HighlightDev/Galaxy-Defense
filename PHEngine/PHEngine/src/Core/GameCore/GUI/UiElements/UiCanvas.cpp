#include "UiCanvas.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/UiSceneProxy/UiCanvasSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiCanvasLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/IAnimatable.h"

#include <algorithm>
#include <json/json.hpp>

using namespace EngineCore;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace EngineCore::Scripts;

namespace EngineCore
{
    namespace GUI
    {
        size_t UiCanvas::s_UId = 0;

        UiCanvas::UiCanvas(const ViewPortInfo &canvasScreenProperties)
            : EngineToLuaReplicatorBase(),
              mUId(s_UId++),
              mName("UiCanvas_" + std::to_string(mUId)),
              mAbsoluteOrigin(glm::ivec2(canvasScreenProperties.OriginX, canvasScreenProperties.OriginY)),
              mWidthHeight(glm::ivec2(canvasScreenProperties.Width, canvasScreenProperties.Height)),
              mChildren(),
              mRegisteredUIds(),
              mRegisteredNames(),
              mIsVisible(true),
              mIsTransformDirty(true),
              mInputSystem(),
              mDescendingByZOrderHierarchyChildren(),
              mOpacityProperty(std::make_shared<EngineObjectProperty<float>>(1.0f, "Opacity", [=](const float opacity)
                                                                             { UpdateOpacityProperty(); }))
        {
            LogInfo("UiCanvas::ctor => ", mUId);

            mProperties.emplace("Opacity", mOpacityProperty);
        }

        UiCanvas::~UiCanvas()
        {
            WindowSizeChangedEvent::GetInstance()->RemoveListener(WindowSizeChangedEvent::GetInstanceId());
        }

        void UiCanvas::Initialize()
        {
            WindowSizeChangedEvent::GetInstance()->AddListener(std::dynamic_pointer_cast<UiCanvas>(shared_from_this()));
        }

        void UiCanvas::InitializeInputSystem()
        {
            if (!mInputSystem)
            {
                mInputSystem = std::make_shared<UiInputSystem>(std::static_pointer_cast<UiCanvas>(shared_from_this()));
                mInputSystem->Initialize();
            }
        }

        void UiCanvas::DeinitializeInputSystem()
        {
            if (mInputSystem)
            {
                mInputSystem.reset();
            }
        }

        std::shared_ptr<LuaProxy> UiCanvas::ReplicateLuaProxy()
        {
            return std::make_shared<UiCanvasLuaProxy>(std::static_pointer_cast<UiCanvas>(shared_from_this()));
        }

        void UiCanvas::SetIsSceneProxyReady(const bool isReady)
        {
            mIsSceneProxyReady.store(isReady, std::memory_order::memory_order_seq_cst);
        }

        bool UiCanvas::GetIsSceneProxyReady() const
        {
            return mIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst);
        }

        void UiCanvas::SetIsLuaProxyReady(const bool isReady)
        {
            mIsLuaProxyReady.store(isReady, std::memory_order::memory_order_seq_cst);
        }

        bool UiCanvas::GetIsLuaProxyReady() const
        {
            return mIsLuaProxyReady.load(std::memory_order::memory_order_seq_cst);
        }

        void UiCanvas::InitLuaProxy(const std::shared_ptr<Scene> &sceneSp)
        {
            static constexpr uint64_t functionId = Hash64_CT("UiCanvas::InitLuaProxy");
            assert(sceneSp);
            mIsPendingToAddLuaProxy = false;
            const auto &luaProxy = ReplicateLuaProxy();
            luaProxy->SetSceneWp(sceneSp);
            luaProxy->SetLuaScriptProcessor(GetLuaScriptProcessorWp());

            sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(eEnqueueJobPolicy::PUSH_ANYWAY, GetUId(), functionId, [this, luaScriptProcessorWp = GetLuaScriptProcessorWp(), luaProxy]
                                                                             {
                    if (const auto &luaProcessorSp = luaScriptProcessorWp.lock())
                    {
                    luaProcessorSp->AddLuaProxy(luaProxy);
                    SetIsLuaProxyReady(true);
                    } });
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

        bool UiCanvas::IsInputSystemInitialized() const
        {
            return mInputSystem != nullptr;
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

        std::shared_ptr<EngineObjectPropertyBase> UiCanvas::GetPropertyByName(const std::string &propName) const
        {
            if (mProperties.count(propName))
            {
                return mProperties.at(propName);
            }

            return nullptr;
        }

        std::shared_ptr<Animator> UiCanvas::GetAnimator() const
        {
            return mAnimator;
        }

        void UiCanvas::CreateAnimator()
        {
            if (!mAnimator)
            {
                mAnimator = std::make_shared<Animator>(std::dynamic_pointer_cast<IAnimatable>(shared_from_this()));
            }
        }

        void UiCanvas::AddAnimation(const std::string &animationName, const AnimationData &animationData)
        {
            if (!mAnimator)
            {
                CreateAnimator();
            }
            mAnimator->AddAnimation(animationName, animationData);
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
                mIsPropertiesShouldBeUpdatedOnLuaThread = true;
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
            if (mCanvasZOrder != z_order)
            {
                mCanvasZOrder = z_order;
                mIsPropertiesShouldBeUpdatedOnRenderThread = true;
            }
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

        BoundingBox2D<glm::ivec2> UiCanvas::GetBoundingArea() const
        {
            const auto &halfExtent = mWidthHeight / 2;
            return BoundingBox2D<glm::ivec2>(mAbsoluteOrigin + halfExtent, halfExtent);
        }

        void UiCanvas::SetAnchor(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor, const std::string &dstUiItemName)
        {
        }

        void UiCanvas::SetAnchorMargin(const eUiAnchor anchor, const int32_t anchorMargin)
        {
        }

        void UiCanvas::SetHorizontalCenterOffset(const int32_t offset)
        {
        }

        void UiCanvas::SetVerticalCenterOffset(const int32_t offset)
        {
        }

        void UiCanvas::AddUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            RegisterUiItem(uiItem->GetUId(), uiItem->GetName());
            mChildren.emplace_back(uiItem);
            uiItem->OnRegistered();
            SetIsTransformDirty(true);
            CollectChildrenWithDescendingZOrder();
        }

        void UiCanvas::ProcessEvent(const WindowSizeChangedEvent::EventData_t &data)
        {
            const glm::ivec4 newViewPortInfo = static_cast<glm::ivec4>(std::get<0>(data));
            mWidthHeight = glm::ivec2(newViewPortInfo.z, newViewPortInfo.w);
            SetIsTransformDirty(true);
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

            if (mIsPropertiesShouldBeUpdatedOnRenderThread && mIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                SyncDataOnRenderThread();
                mIsPropertiesShouldBeUpdatedOnRenderThread = false;
            }

            if (mIsPropertiesShouldBeUpdatedOnLuaThread && mIsLuaProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                SyncDataOnLuaThread();
                mIsPropertiesShouldBeUpdatedOnLuaThread = false;
            }

            for (const auto &child : mChildren)
            {
                child->UnpausableTick(deltaTime);
            }

            if (mInputSystem && mIsVisible)
            {
                mInputSystem->UnpausableTick(deltaTime);
            }

            if (mAnimator)
            {
                mAnimator->UnpausableTick(deltaTime);
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
            const auto foundIt = std::find_if(mChildren.begin(), mChildren.end(), [name](const auto &child)
                                              { return child->GetName() == name; });
            return foundIt != mChildren.end() ? (*foundIt) : nullptr;
        }

        std::shared_ptr<IUiTransformable> UiCanvas::TryFindHierarchyChildByName(const std::string &name) const
        {
            for (const auto &child : mChildren)
            {
                if (child->GetName() == name)
                {
                    return child;
                }
                else if (const auto foundChild = child->TryFindHierarchyChildByName(name))
                {
                    return foundChild;
                }
            }
            return nullptr;
        }

        std::shared_ptr<IUiTransformable> UiCanvas::TryFindHierarchyChildByUId(const uint32_t uid) const
        {
            for (const auto &child : mChildren)
            {
                if (child->GetUId() == uid)
                {
                    return child;
                }
                else if (const auto foundChild = child->TryFindHierarchyChildByUId(uid))
                {
                    return foundChild;
                }
            }
            return nullptr;
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
                if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                {
                    sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [sceneRenderer, uid = GetUId(), isVisible = mIsVisible, absoluteOrigin = mAbsoluteOrigin, widthHeight = mWidthHeight, canvasZOrder = mCanvasZOrder]()
                                                                                        {
                        if (const auto &canvasProxy = sceneRenderer->GetCanvasSceneProxyByProxyId(uid))
                        {
                            canvasProxy->SetIsVisible(isVisible);
                            canvasProxy->SetAbsoluteOrigin(absoluteOrigin);
                            canvasProxy->SetWidthHeight(widthHeight);
                            canvasProxy->SetCanvasZOrder(canvasZOrder);
                        } });
                }
            }
        }

        void UiCanvas::SyncDataOnLuaThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiCanvas::SyncDataOnLuaThread");
            if (const auto &sceneSp = mScene.lock())
            {
                if (const auto &luaScriptProcessorSp = GetLuaScriptProcessorWp().lock())
                {
                    sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetReplicatorId(), functionId, [luaScriptProcessorSp, luaProxyId = GetLuaProxyId(), isVisible = mIsVisible, canvasZOrder = mCanvasZOrder]()
                                                                                     {
                        if (const auto &canvasProxy = std::static_pointer_cast<UiCanvasLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId)))
                        {
                            canvasProxy->SetIsVisible_FromGameThread(isVisible);
                            canvasProxy->SetCanvasZOrder_FromGameThread(canvasZOrder);
                        } });
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

        void UiCanvas::SyncFromLuaJsonProperties(const std::string &luaJsonPropsStr)
        {
            const auto &jsonObj = nlohmann::json::parse(luaJsonPropsStr);
            if (jsonObj.contains("visible"))
            {
                const auto isVisible = jsonObj["visible"].get<bool>();
                if (mIsVisible != isVisible)
                {
                    mIsVisible = isVisible;
                    mIsPropertiesShouldBeUpdatedOnRenderThread = true;
                }
            }
            if (jsonObj.contains("canvas_z_order"))
            {
                const auto zOrder = jsonObj["canvas_z_order"].get<size_t>();
                if (mCanvasZOrder != zOrder)
                {
                    mCanvasZOrder = zOrder;
                    mIsPropertiesShouldBeUpdatedOnRenderThread = true;
                }
            }
        }

        void UiCanvas::UpdateOpacityProperty()
        {
            if (mIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                static constexpr uint64_t functionId = Hash64_CT("UiCanvas::UpdateOpacityProperty");
                if (const auto &sceneSp = mScene.lock())
                {
                    if (const auto &sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                    {
                        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [sceneRendererSp, uid = GetUId(), opacity = mOpacityProperty->GetValue()]()
                                                                                            {
                            if (const auto &canvasProxy = sceneRendererSp->GetCanvasSceneProxyByProxyId(uid))
                            {
                                canvasProxy->SetOverlayOpacity(opacity); 
                            } });
                    }
                }
            }
        }

        void UiCanvas::CleanUp()
        {
            for (const auto &child : mChildren)
            {
                child->CleanUp();
            }
            mChildren.clear();
        }

        void UiCanvas::RemoveSceneProxy()
        {
            if (mIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                if (const auto &sceneSp = mScene.lock())
                {
                    if (const auto &sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                    {
                        // Clear all children scene proxies, because all of them relate to current canvas
                        for (const auto &uiItemChild : mChildren)
                        {
                            sceneRendererSp->UnregisterUiSceneProxy_OnRenderThread(uiItemChild->GetUId(), GetUId());
                            uiItemChild->SetIsSceneProxyReady(false);
                        }

                        sceneRendererSp->UnregisterUiCanvasProxy_OnRenderThread(GetUId());
                        SetIsSceneProxyReady(false);
                    }
                }
            }
        }

        void UiCanvas::RemoveFromReplicators()
        {
            if (const auto &sceneSp = mScene.lock())
            {
                sceneSp->UnregisterEngineToLuaReplicator(GetReplicatorId());
            }
        }

        void UiCanvas::RemoveLuaProxy()
        {
            if (mIsLuaProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                if (const auto &luaProcessorSp = GetLuaScriptProcessorWp().lock())
                {
                    luaProcessorSp->RemoveLuaProxy(GetLuaProxyId());
                }
                SetIsLuaProxyReady(false);
            }
        }
    }
}