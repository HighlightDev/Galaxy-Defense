#pragma once

#include "IUiTransformable.h"
#include "Transform2D/UiAnchorType.h"
#include "Transform2D/UiAnchorData.h"
#include "Transform2D/BoundingBox2D.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/GUI/UiInputSystem/IUiMouseInputReceivable.h"
#include "Core/GameCore/ScriptingCore/EngineToLuaReplicatorBase.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/mat4x4.hpp>

using namespace EngineCore::Scripts;

namespace EngineCore
{
    class Scene;
    namespace GUI
    {
        class UiCanvas;

        class UiItemBase
            : public EngineToLuaReplicatorBase,
              public IUiTransformable,
              public ITickable
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

            BoundingBox2D mBoundingArea;

            std::unordered_map<eUiAnchor /*src anchor*/, UiAnchorData> mAnchors;

            int32_t mHorizontalCenterOffset;

            int32_t mVerticalCenterOffset;

            std::weak_ptr<IUiTransformable> mParent;

            std::weak_ptr<UiCanvas> mParentCanvas;

            std::vector<std::shared_ptr<UiItemBase>> mChildren;

            bool mIsVisible;

            bool mIsTransformDirty;

            bool mIsPropertiesShouldBeUpdatedOnRenderThread;

            bool mIsPropertiesShouldBeUpdatedOnLuaThread;

            std::shared_ptr<IUiMouseInputReceivable> mMouseInputReceiver;

        public:
            UiItemBase();

            void SetParents(const std::weak_ptr<UiCanvas> &parentCanvas, const std::weak_ptr<IUiTransformable> &parent);

            void SetParents(const std::string& uiCanvasName, const std::string& uiWidgetParentName);

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
            std::string GetName() const override;
            size_t GetUId() const override;
            BoundingBox2D GetBoundingArea() const override;
            bool IsTransformDirty() const override;
            std::vector<std::shared_ptr<UiItemBase>> GetAllChildren() const;
            std::weak_ptr<::EngineCore::Scene> GetScene() const override;

            void SetZOrder(const size_t z_order) override;
            void SetWidth(const size_t width) override;
            void SetHeight(const size_t height) override;
            void SetIsVisible(const bool isVisible) override;
            void SetAnchor(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor, const std::string &dstUiItemName) override;
            void SetAnchorMargin(const eUiAnchor anchor, const int32_t anchorMargin) override;
            void SetHorizontalCenterOffset(const int32_t offset) override;
            void SetVerticalCenterOffset(const int32_t offset) override;
            void AddUiItem(const std::shared_ptr<UiItemBase> &uiItem) override;
            void RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem);
            void RegisterUiItem(const size_t uiId, const std::string &uiItemName) override;
            void UnregisterUiItem(const size_t uiId, const std::string &uiItemName) override;
            std::shared_ptr<IUiTransformable> TryFindChildByName(const std::string &name) const override;

            // Implementation of ITickable
            void Tick(const float deltaTime) override;
            void UnpausableTick(const float deltaTime) override;

        protected:
            void SetIsTransformDirty(const bool isDirty);

            void SetChildrenIsVisible(const bool isVisible);

            void SetIsPropertiesShouldBeUpdatedOnRenderThread(const bool update);

            void SetIsPropertiesShouldBeUpdatedOnLuaThread(const bool update);

            void UpdateAnchorTransform();

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

            void SetAbsoluteOrigin(const glm::ivec2 &transform) override;
        };
    }
}