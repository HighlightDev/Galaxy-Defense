#pragma once

#include "IUiTransformable.h"
#include "Transform2D/UiAnchorType.h"
#include "Transform2D/UiAnchorData.h"
#include "Transform2D/BoundingBox2D.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/GUI/UiInputSystem/IUiMouseInputReceivable.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/mat4x4.hpp>

// On win32 cause compilation error
#undef ABSOLUTE
#undef RELATIVE

namespace EngineCore
{
    class Scene;
    namespace GUI
    {
        class UiCanvas;
        enum class eUiItemPositioningType
        {
            ABSOLUTE,
            RELATIVE,
            ANCHORS
        };

        class UiItemBase : public IUiTransformable,
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

            std::shared_ptr<IUiMouseInputReceivable> mMouseInputReceiver;

        public:
            explicit UiItemBase(const std::weak_ptr<UiCanvas> &parentCanvas, const std::weak_ptr<IUiTransformable> &parent);

            virtual ~UiItemBase() = default;

            virtual void OnRegistered() = 0;
            virtual void OnUnregistered() = 0;

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
            const std::weak_ptr<UiCanvas> &GetParentCanvas() const;
            std::vector<std::shared_ptr<UiItemBase>> GetAllChildren() const;
            std::weak_ptr<::EngineCore::Scene> GetScene() const override;
            std::shared_ptr<IUiMouseInputReceivable> GetMouseInputReceiver() const;

            void SetAbsoluteOrigin(const glm::ivec2 &transform) override;
            void SetZOrder(const size_t z_order) override;
            void SetWidth(const size_t width) override;
            void SetHeight(const size_t height) override;
            void SetIsVisible(const bool isVisible) override;
            void SetAnchor(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor, const std::string &dstUiItemName) override;
            void SetAnchorMargin(const eUiAnchor anchor, const int32_t anchorMargin) override;
            void SetHorizontalCenterOffset(const int32_t offset) override;
            void SetVerticalCenterOffset(const int32_t offset) override;
            void SetMouseInputReceiver(const std::shared_ptr<IUiMouseInputReceivable> &inputReceiver);

            void AddUiItem(const std::shared_ptr<UiItemBase> &uiItem);
            void RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem);
            void RegisterUiItem(const size_t uiId, const std::string &uiItemName) override;
            void UnregisterUiItem(const size_t uiId, const std::string &uiItemName) override;

            std::shared_ptr<IUiTransformable> TryFindChildByName(const std::string &name) const override;

            std::shared_ptr<IUiTransformable> TryFindAncestryUiItem(const std::string &name) const;

            void Tick(const float deltaTime) override;

            void UnpausableTick(const float deltaTime) override;

            bool IsTransformDependentToUiItem(const std::string &uiItemName) const;

            void GetDependentByTransformChildren(const std::string &nameOfRelatedUiItem, std::vector<std::shared_ptr<UiItemBase>> &affectedUiItems);

            void UpdateAnchorTransform();
            void UpdateDependentChildrenAnchorTransform();

            void CollectAllHierarchyChildren(std::vector<std::shared_ptr<UiItemBase>> &inCollection) const;

            // Input events
            void OnMousePositionChanged(const glm::ivec2 &mouseCursorPosition);
            void OnMouseReleased(const glm::ivec2 &mouseCursorPosition);
            void OnMousePressed(const glm::ivec2 &mouseCursorPosition);
            void OnMouseClicked(const glm::ivec2 &mouseCursorPosition);

            virtual void OnPropertiesShouldBeUpdatedOnRenderThread();

        protected:
            void SetIsTransformDirty(const bool isDirty);

            void SetChildrenIsVisible(const bool isVisible);

            void SetIsPropertiesShouldBeUpdated(const bool update);

        private:
            void TransformChanged();

            void RebuildBoundingArea();
            void RebuildNormalizedTransform();

            void RecalculateAnchorPositions();
            void CalculateHorizontalAnchorPositions();
            void CalculateVerticalAnchorPositions();

            void SyncDataOnRenderThread();
        };
    }
}