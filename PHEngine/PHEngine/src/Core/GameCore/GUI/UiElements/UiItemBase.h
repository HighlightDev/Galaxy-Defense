#pragma once

#include "IUiTransformable.h"
#include "Transform2D/UiAnchorType.h"
#include "Transform2D/UiAnchorData.h"
#include "Transform2D/BoundingBox2D.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/GUI/UiInputSystem/IUiMouseInputReceivable.h"
#include "Core/GameCore/ScriptingCore/EngineToLuaReplicatorBase.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/IAnimatable.h"
#include "Core/GameCore/EngineObjectProperty.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/mat4x4.hpp>
#include <atomic>

using namespace EngineCore::Scripts;

namespace EngineCore
{
    class Scene;
    namespace GUI
    {
        class UiCanvas;
        class Animator;
        class SequenceAnimator;

        class UiItemBase
            : public EngineToLuaReplicatorBase,
              public IUiTransformable,
              public ITickable,
              public IAnimatable
        {
            size_t mUId;

            static size_t s_UIds;

            std::string mName;

        protected:
            /**
             *@brief bottom left corner of ui item
             **/
            glm::ivec2 mAbsoluteOrigin;

            glm::vec2 mNormalizedTranslation;

            glm::vec2 mNormalizedScale;

            size_t mZOrder;

            size_t mWidth;

            size_t mHeight;

            BoundingBox2D<glm::ivec2> mBoundingArea;

            std::unordered_map<eUiAnchor /*src anchor*/, UiAnchorData> mAnchors;

            int32_t mHorizontalCenterOffset;

            int32_t mVerticalCenterOffset;

            std::weak_ptr<IUiTransformable> mParent;

            std::weak_ptr<UiCanvas> mParentCanvas;

            std::vector<std::shared_ptr<UiItemBase>> mChildren;

            std::atomic<bool> mIsSceneProxyReady{false}; // only when this value is true - data could be updated on render thread

            std::atomic<bool> mIsLuaProxyReady{false}; // only when this value is true - data could be updated on lua thread

            bool mIsVisible;

            bool mIsVisibleDirty{false};

            bool mCanInterceptMouseInputEvents;

            bool mIsTransformDirty;

            bool mIsPropertiesShouldBeUpdatedOnRenderThread;

            bool mIsPropertiesShouldBeUpdatedOnLuaThread;

            std::shared_ptr<IUiMouseInputReceivable> mMouseInputReceiver;

            std::unordered_map<std::string, std::shared_ptr<::EngineObjectPropertyBase>> mProperties;

            std::shared_ptr<::EngineCore::GUI::Animator> mAnimator;

            std::shared_ptr<::EngineCore::GUI::SequenceAnimator> mSequenceAnimator;

            std::shared_ptr<EngineObjectProperty<float>> mScaleProperty;

            std::shared_ptr<EngineObjectProperty<int32_t>> mVerticalCenterOffsetProperty;

            std::shared_ptr<EngineObjectProperty<int32_t>> mHorizontalCenterOffsetProperty;

        public:
            explicit UiItemBase(const std::string &name);

            void Initialize();

            void SetParents(const std::weak_ptr<UiCanvas> &parentCanvas, const std::weak_ptr<IUiTransformable> &parent);

            void SetParents(const std::string &uiCanvasName, const std::string &uiWidgetParentName);

            void SetIsSceneProxyReady(const bool isSceneProxyReady);

            void SetIsLuaProxyReady(const bool isLuaProxyReady);

            virtual std::string GetUiTypeString() const = 0;

            virtual void OnRegistered() = 0;

            virtual void OnUnregistered() = 0;

            virtual void OnPropertiesShouldBeUpdatedOnRenderThread();

            virtual void OnPropertiesShouldBeUpdatedOnLuaThread();

            bool IsTransformDependentToUiItem(const std::string &uiItemName) const;

            std::shared_ptr<IUiMouseInputReceivable> GetMouseInputReceiver() const;

            void SetMouseInputReceiver(const std::shared_ptr<IUiMouseInputReceivable> &inputReceiver);

            const std::weak_ptr<UiCanvas> &GetParentCanvas() const;

            void GetDependentByTransformChildren(const std::string &nameOfRelatedUiItem, std::vector<std::shared_ptr<UiItemBase>> &affectedUiItems);

            void UpdateDependentChildrenAnchorTransform();

            void CollectAllHierarchyChildren(std::vector<std::shared_ptr<UiItemBase>> &inCollection) const;

            int32_t GetHorizontalCenterOffset() const;

            int32_t GetVerticalCenterOffset() const;

            // Input events
            void OnMousePositionChanged(const glm::ivec2 &mouseCursorPosition);
            void OnMouseReleased(const glm::ivec2 &mouseCursorPosition);
            void OnMousePressed(const glm::ivec2 &mouseCursorPosition);
            void OnMouseClicked(const glm::ivec2 &mouseCursorPosition);

            // Implementation of EngineToLuaReplicatorBase
            void SyncFromLuaJsonProperties(const std::string &luaJsonPropsStr) override;

            // Implementation of IUiTransformable
            const glm::ivec2 &GetAbsoluteOrigin() const override;
            size_t GetZOrder() const override;
            size_t GetWidth() const override;
            size_t GetHeight() const override;
            glm::vec2 GetNormalizedTranslation() const override;
            glm::vec2 GetNormalizedScale() const override;
            std::weak_ptr<IUiTransformable> GetRootParent() const override;
            std::weak_ptr<IUiTransformable> GetParent() const override;
            bool IsVisible() const override;
            bool GetIfCanInterceptMouseInputEvents() const override;
            std::string GetName() const override;
            size_t GetUId() const override;
            BoundingBox2D<glm::ivec2> GetBoundingArea() const override;
            bool IsTransformDirty() const override;
            std::vector<std::shared_ptr<UiItemBase>> GetAllChildren() const;
            std::weak_ptr<::EngineCore::Scene> GetScene() const override;
            const std::unordered_map<eUiAnchor /*src anchor*/, UiAnchorData> &GetAnchors() const;
            bool IsVisibleDirty() const;

            void SetAbsoluteOrigin(const glm::ivec2 &transform) override;
            void SetZOrder(const size_t z_order) override;
            void SetWidth(const size_t width) override;
            void SetHeight(const size_t height) override;
            void SetIsVisible(const bool isVisible) override;
            void SetIfCanInterceptMouseInputEvents(const bool intercepts) override;
            void SetAnchor(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor, const std::string &dstUiItemName) override;
            void SetAnchorMargin(const eUiAnchor anchor, const int32_t anchorMargin) override;
            void SetHorizontalCenterOffset(const int32_t offset) override;
            void SetVerticalCenterOffset(const int32_t offset) override;
            void AddUiItem(const std::shared_ptr<UiItemBase> &uiItem) override;
            void RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem);
            void RegisterUiItem(const size_t uiId, const std::string &uiItemName) override;
            void UnregisterUiItem(const size_t uiId, const std::string &uiItemName) override;
            std::shared_ptr<IUiTransformable> TryFindChildByName(const std::string &name) const override;
            std::shared_ptr<IUiTransformable> TryFindHierarchyChildByName(const std::string &name) const;
            std::shared_ptr<IUiTransformable> TryFindHierarchyChildByUId(const uint32_t uid) const;

            // Implementation of ITickable
            void Tick(const float deltaTime) override;
            void UnpausableTick(const float deltaTime) override;

            std::shared_ptr<::EngineObjectPropertyBase> GetPropertyByName(const std::string &propName) const override;

            std::shared_ptr<::EngineCore::GUI::Animator> GetAnimator() const override;

            std::shared_ptr<::EngineCore::GUI::SequenceAnimator> GetSequenceAnimator() const override;

            void CreateAnimator() override;

            void CreateSequenceAnimator() override;

            void AddAnimation(const std::string &animationName, const ::EngineCore::GUI::AnimationData &animationData) override;

            void AddSequenceAnimation(const std::string &animationName, const ::EngineCore::GUI::AnimationSequence &animationSequence) override;

            void InitLuaProxy(const std::shared_ptr<Scene> &sceneSp) override;

            void CleanUp() override;

            bool CheckIfInterceptsMouseEvent(const glm::ivec2 &currentMousePosition) const;
            
        protected:
            void SetIsTransformDirty(const bool isDirty);

            void SetIsVisibleDirty(const bool isDirty);

            void SetChildrenIsVisible(const bool isVisible);

            void SetIsPropertiesShouldBeUpdatedOnRenderThread(const bool update);

            void SetIsPropertiesShouldBeUpdatedOnLuaThread(const bool update);

            virtual void UpdateAnchorTransform();

            std::shared_ptr<IUiTransformable> TryFindAncestryUiItem(const std::string &name) const;

        private:
            void TransformChanged();

            void RebuildBoundingArea();
            void RebuildNormalizedTransform();

            void RecalculateAnchorPositions();
            void CalculateHorizontalAnchorPositions();
            void CalculateVerticalAnchorPositions();

            void SyncDataOnRenderThread();

            void SyncDataOnLuaThread();

            void UpdateScaleProperty();

            void UpdateCenterOffsetProperties();

            void RemoveFromReplicators();

            void RemoveLuaProxy();
        };
    }
}