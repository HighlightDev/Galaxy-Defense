#pragma once

#include "IUiTransformable.h"
#include "Transform2D/UiAnchorType.h"
#include "Transform2D/UiAnchorData.h"
#include "Transform2D/BoundingBox2D.h"
#include "Core/GameCore/ITickable.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/mat4x4.hpp>

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

            std::weak_ptr<IUiTransformable> mParent;
            std::weak_ptr<UiCanvas> mParentCanvas;

            std::vector<std::shared_ptr<UiItemBase>> mChildren;

            eUiItemPositioningType mUiPositioningType{eUiItemPositioningType::ANCHORS};

            bool mIsVisible;

            bool mIsTransformDirty;

        public:
            explicit UiItemBase(const std::weak_ptr<UiCanvas> &parentCanvas, const std::weak_ptr<IUiTransformable> &parent);

            virtual ~UiItemBase() = default;

            virtual const glm::ivec2 &GetAbsoluteOrigin() const override;
            virtual size_t GetZOrder() const override;
            virtual size_t GetWidth() const override;
            virtual size_t GetHeight() const override;
            virtual glm::vec2 GetNormalizedTranslation() const override;
            virtual glm::vec2 GetNormalizedScale() const override;
            virtual std::weak_ptr<IUiTransformable> GetRootParent() const override;
            virtual std::weak_ptr<IUiTransformable> GetParent() const override;
            virtual bool IsVisible() const override;
            virtual std::string GetName() const override;
            virtual size_t GetUId() const override;
            virtual BoundingBox2D GetBoundingArea() const override;
            bool IsTransformDirty() const override;
            const std::weak_ptr<UiCanvas> &GetParentCanvas() const;
            std::vector<std::shared_ptr<UiItemBase>> GetAllChildren() const;
            virtual std::weak_ptr<::EngineCore::Scene> GetScene() const override;

            virtual void SetAbsoluteOrigin(const glm::ivec2 &transform) override;
            virtual void SetZOrder(const size_t z_order) override;
            virtual void SetWidth(const size_t width) override;
            virtual void SetHeight(const size_t height) override;
            virtual void SetIsVisible(const bool isVisible) override;
            virtual void SetAnchor(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor, const std::string &dstUiItemName) override;
            virtual void SetAnchorMargin(const eUiAnchor anchor, const int32_t anchorMargin) override;

            void AddUiItem(const std::shared_ptr<UiItemBase> &uiItem);
            void RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem);
            virtual void RegisterUiItem(const size_t uiId, const std::string &uiItemName) override;
            virtual void UnregisterUiItem(const size_t uiId, const std::string &uiItemName) override;

            virtual void OnRegistered();
            virtual void OnUnregistered();

            virtual std::shared_ptr<IUiTransformable> TryFindChildByName(const std::string &name) const override;

            std::shared_ptr<IUiTransformable> TryFindAncestryUiItem(const std::string &name) const;

            virtual void Tick(const float deltaTime) override;

            bool IsTransformDependentToUiItem(const std::string &uiItemName) const;

            void GetDependentByTransformChildren(const std::string &nameOfRelatedUiItem, std::vector<std::shared_ptr<UiItemBase>> &affectedUiItems);

            void UpdateAnchorTransform();
            void UpdateDependentChildrenAnchorTransform();

        protected:
            void SetIsTransformDirty(const bool isDirty);

            void SetChildrenIsVisible(const bool isVisible);

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