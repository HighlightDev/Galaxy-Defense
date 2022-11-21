#pragma once

#include "UiItemBase.h"
#include "IUiTransformable.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

#include <unordered_set>

using namespace Graphics;

namespace EngineCore
{
    namespace GUI
    {
        class UiCanvas : public IUiTransformable
        {
        private:
            Transform2D mAbsoluteOrigin;
            Transform2D mRelativeOrigin;
            glm::ivec2 mWidthHeight;

        protected:
            std::vector<std::shared_ptr<UiItemBase>> mChildren;
            std::unordered_set<size_t> mRegisteredUiItems;
            std::vector<std::shared_ptr<UiItemBase>> mSortedChildren;

        public:
            explicit UiCanvas(const ViewPortInfo &canvasScreenProperties);

            virtual const Transform2D &GetAbsoluteOrigin() const override;
            virtual const Transform2D &GetRelativeOrigin() const override;
            virtual size_t GetZOrder() const override;
            virtual size_t GetWidth() const override;
            virtual size_t GetHeight() const override;

            virtual void SetAbsoluteOrigin(const Transform2D &transform) override;
            virtual void SetRelativeOrigin(const Transform2D &transform) override;
            virtual void SetZOrder(const size_t z_order) override;
            virtual void SetWidth(const size_t width) override;
            virtual void SetHeight(const size_t height) override;

            void AddUiItem(const std::shared_ptr<UiItemBase> &uiItem);
            void RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem);

            void Render();

        private:
            
            void SortChildrenByZOrder();
            void UpdateHierarchyTransform();

        protected:
            void RegisterUiItem(const size_t uiId);
            void UnregisterUiItem(const size_t uiId);
            virtual void UpdateSortedChildren() override;
        };
    }
}