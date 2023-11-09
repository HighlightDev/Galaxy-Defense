#pragma once

#include "UiItemBase.h"
#include "IUiTransformable.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/GUI/UiInputSystem/UiInputSystem.h"
#include "Core/GameCore/ScriptingCore/EngineToLuaReplicatorBase.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/IAnimatable.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/AnimationData.h"
#include "Core/GameCore/Event/WindowSizeChangedEvent.h"

#include <unordered_set>
#include <memory>
#include <atomic>

using namespace Graphics;
using namespace EngineCore::Scripts;
using namespace Event;

namespace Graphics::Proxy
{
    class UiCanvasSceneProxy;
}

namespace EngineCore::Scripts
{
    class LuaProxy;
}

struct EngineObjectPropertyBase;

namespace EngineCore
{
    class Scene;

    namespace GUI
    {
        class UiCanvas : public EngineToLuaReplicatorBase,
                         public IUiTransformable,
                         public ITickable,
                         public IAnimatable,
                         public WindowSizeChangedEvent
        {
        private:
            static size_t s_UId;

            size_t mUId;

            std::string mName;

            std::weak_ptr<::EngineCore::Scene> mScene;

            glm::ivec2 mAbsoluteOrigin;

            glm::ivec2 mWidthHeight;

            bool mIsVisible;

            bool mIsTransformDirty;

            bool mIsPropertiesShouldBeUpdatedOnRenderThread{false};

            bool mIsPropertiesShouldBeUpdatedOnLuaThread{false};

            std::atomic<bool> mIsSceneProxyReady{false}; // only when this value is true - data could be updated on render thread

            std::atomic<bool> mIsLuaProxyReady{false};

            std::shared_ptr<UiInputSystem> mInputSystem;

            bool mWasHoveredLastFrame{false};

            bool mMouseButtonWasPressedLastFrame{false};

            std::vector<std::weak_ptr<UiItemBase>> mDescendingByZOrderHierarchyChildren;

            bool mIsCreatedFromLua{false};

            std::shared_ptr<EngineObjectProperty<float>> mOpacityProperty;

            std::unordered_map<std::string, std::shared_ptr<::EngineObjectPropertyBase>> mProperties;

            std::shared_ptr<Animator> mAnimator;

            size_t mCanvasZOrder{0}; // order of rendering canvases

        protected:
            std::vector<std::shared_ptr<UiItemBase>> mChildren;

            std::unordered_set<size_t> mRegisteredUIds;
            std::unordered_set<std::string> mRegisteredNames;

        public:
            explicit UiCanvas(const ViewPortInfo &canvasScreenProperties);

            ~UiCanvas() override;

            void Initialize();

            void SetIsSceneProxyReady(const bool isReady);

            bool GetIsSceneProxyReady() const;

            void SetIsLuaProxyReady(const bool isReady);

            bool GetIsLuaProxyReady() const;

            void InitLuaProxy(const std::shared_ptr<::EngineCore::Scene> &sceneSp) override;

            void InitializeInputSystem();

            void DeinitializeInputSystem();

            std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

            void SyncFromLuaJsonProperties(const std::string &luaJsonPropsStr) override;

            size_t GetUId() const override;
            const glm::ivec2 &GetAbsoluteOrigin() const override;
            size_t GetZOrder() const override;
            size_t GetWidth() const override;
            size_t GetHeight() const override;
            glm::vec2 GetNormalizedTranslation() const override;
            glm::vec2 GetNormalizedScale() const override;
            std::weak_ptr<IUiTransformable> GetRootParent() const override;
            std::weak_ptr<IUiTransformable> GetParent() const override;
            std::string GetName() const override;
            std::weak_ptr<::EngineCore::Scene> GetScene() const override;
            BoundingBox2D GetBoundingArea() const override;
            bool IsVisible() const override;
            bool IsTransformDirty() const override;
            bool IsInputSystemInitialized() const;
            std::shared_ptr<IUiTransformable> TryFindChildByName(const std::string &name) const override;
            std::shared_ptr<IUiTransformable> TryFindHierarchyChildByName(const std::string &name) const;
            std::shared_ptr<IUiTransformable> TryFindHierarchyChildByUId(const uint32_t uid) const;

            void SetAbsoluteOrigin(const glm::ivec2 &transform) override;
            void SetZOrder(const size_t z_order) override;
            void SetWidth(const size_t width) override;
            void SetHeight(const size_t height) override;
            void SetIsVisible(const bool isVisible) override;
            void SetScene(const std::weak_ptr<::EngineCore::Scene> &sceneWp);

            void AddUiItem(const std::shared_ptr<UiItemBase> &uiItem) override;
            void RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem);

            void Tick(const float deltaTime) override;

            void UnpausableTick(const float deltaTime) override;

            std::shared_ptr<::Graphics::Proxy::UiCanvasSceneProxy> CreateUiCanvasSceneProxy() const;

            std::vector<std::shared_ptr<UiItemBase>> GetDependentByTransformChildren(const std::string &nameOfChangedTransformUiItem) const;
            void CollectChildrenWithDescendingZOrder();

            // Input events
            void OnMousePositionChanged(const glm::ivec2 &mouseCursorPosition);
            void OnMouseReleased(const glm::ivec2 &mouseCursorPosition);
            void OnMousePressed(const glm::ivec2 &mouseCursorPosition);
            void OnMouseClicked(const glm::ivec2 &mouseCursorPosition);

            std::shared_ptr<::EngineObjectPropertyBase> GetPropertyByName(const std::string &propName) const override;

            std::shared_ptr<Animator> GetAnimator() const override;

            void CreateAnimator() override;

            void AddAnimation(const std::string &animationName, const AnimationData &animationData) override;

            void CleanUp() override;

        protected:
            void ProcessEvent(const WindowSizeChangedEvent::EventData_t &data) override;

            void RegisterUiItem(const size_t uiId, const std::string &uiItemName);

            void UnregisterUiItem(const size_t uiId, const std::string &uiItemName);

        private:

            void SyncDataOnRenderThread();

            void SyncDataOnLuaThread();

            void SetAnchor(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor, const std::string &dstUiItemName) override;

            void SetAnchorMargin(const eUiAnchor anchor, const int32_t anchorMargin) override;

            void SetHorizontalCenterOffset(const int32_t offset) override;

            void SetVerticalCenterOffset(const int32_t offset) override;

            void SetIsTransformDirty(const bool isDirty);

            void UpdateAnchorTransform();

            void UpdateDependentChildrenAnchorTransform();

            void UpdateOpacityProperty();

            void RemoveSceneProxy();

            void RemoveFromReplicators();

            void RemoveLuaProxy();
        };
    }
}