#pragma once

#include "IUiTransformable.h"
#include "Transform2D/UiAnchorType.h"
#include "Transform2D/BoundingBox2D.h"

#include <memory>
#include <vector>
#include <unordered_map>

namespace EngineCore
{
    namespace GUI
    {
        enum class eUiItemPositioningType
        {
            ABSOLUTE,
            RELATIVE,
            ANCHORS
        };

        class UiItemBase : public IUiTransformable
        {
            size_t mId;
            static size_t s_Ids;

        protected:
            Transform2D mAbsoluteOrigin;
            Transform2D mRelativeOrigin;

            size_t mZOrder;

            size_t mWidth;
            size_t mHeight;

            BoundingBox2D mBoundingArea;

            std::unordered_map<eUiAnchorType, std::shared_ptr<UiItemBase>> mAnchors;

            std::weak_ptr<IUiTransformable> mParent;

            std::vector<std::shared_ptr<UiItemBase>> mChildren;

            eUiItemPositioningType mUiPositioningType{eUiItemPositioningType::RELATIVE};

        public:
            explicit UiItemBase(const std::weak_ptr<IUiTransformable> &parent = std::weak_ptr<IUiTransformable>());

            virtual ~UiItemBase() = default;

            virtual const Transform2D &GetAbsoluteOrigin() const override;
            virtual const Transform2D &GetRelativeOrigin() const override;
            virtual size_t GetZOrder() const override;
            virtual size_t GetWidth() const override;
            virtual size_t GetHeight() const override;
            size_t GetId() const;

            virtual void SetAbsoluteOrigin(const Transform2D &transform) override;
            virtual void SetRelativeOrigin(const Transform2D &transform) override;
            virtual void SetZOrder(const size_t z_order) override;
            virtual void SetWidth(const size_t width) override;
            virtual void SetHeight(const size_t height) override;
            void SetUiAnchor(const eUiAnchorType anchorType, const std::shared_ptr<UiItemBase> &anchorUiItem);

            void AddUiItem(const std::shared_ptr<UiItemBase> &uiItem);
            void RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem);
            virtual void RegisterUiItem(const size_t uiId) override;
            virtual void UnregisterUiItem(const size_t uiId) override;

            // todo: temporary, should be done by scene proxy on RT
            virtual void Render() {}

            std::vector<std::shared_ptr<UiItemBase>> GetAllChildren() const;

        protected:
            virtual void OnTransformChanged();

            virtual void UpdateHierarchyTransform();

            virtual void UpdateSortedChildren() override;

        private:
            void TransformChanged();

            void RebuildTransform();
            void RebuildBoundingArea();
        };
    }
}