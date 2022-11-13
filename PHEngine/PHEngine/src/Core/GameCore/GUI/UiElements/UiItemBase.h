#pragma once

#include "Transform2D/Transform2D.h"
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

        class UiItemBase
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

            std::weak_ptr<UiItemBase> mParent;

            std::vector<std::shared_ptr<UiItemBase>> mChildren;

            eUiItemPositioningType mUiPositioningType{eUiItemPositioningType::RELATIVE};

        public:
            explicit UiItemBase(const std::weak_ptr<UiItemBase> &parent = std::weak_ptr<UiItemBase>());

            virtual ~UiItemBase() = default;

            const Transform2D &GetAbsoluteOrigin() const;
            const Transform2D &GetRelativeOrigin() const;
            size_t GetZOrder() const;
            size_t GetWidth() const;
            size_t GetHeight() const;
            size_t GetId() const;

            void SetAbsoluteOrigin(const Transform2D &transform);
            void SetRelativeOrigin(const Transform2D &transform);
            void SetZOrder(const size_t z_order);
            void SetWidth(const size_t width);
            void SetHeight(const size_t height);
            void SetUiAnchor(const eUiAnchorType anchorType, const std::shared_ptr<UiItemBase> &anchorUiItem);

        protected:
            virtual void OnTransformChanged();

            virtual void UpdateHierarchyTransform();

        private:
            void TransformChanged();

            void RebuildTransform();
            void RebuildBoundingArea();
        };
    }
}