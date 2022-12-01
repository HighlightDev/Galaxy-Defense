#pragma once

#include "IUiTransformable.h"
#include "Transform2D/UiAnchorType.h"
#include "Transform2D/BoundingBox2D.h"

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

        class UiItemBase : public IUiTransformable
        {
            size_t mUId;
            static size_t s_UIds;

        protected:
            glm::ivec2 mAbsoluteOrigin;

            glm::vec2 mNormalizedTranslation;
            glm::vec2 mNormalizedScale;

            glm::mat4 mTransformMatrix;

            size_t mZOrder;

            size_t mWidth;
            size_t mHeight;

            BoundingBox2D mBoundingArea;

            std::unordered_map<eUiAnchorType, std::shared_ptr<UiItemBase>> mAnchors;

            std::weak_ptr<IUiTransformable> mParent;
            std::weak_ptr<UiCanvas> mParentCanvas;

            std::vector<std::shared_ptr<UiItemBase>> mChildren;

            eUiItemPositioningType mUiPositioningType{eUiItemPositioningType::RELATIVE};

            bool mIsVisible;

        public:
            explicit UiItemBase(const std::weak_ptr<UiCanvas>& parentCanvas, const std::weak_ptr<IUiTransformable> &parent = std::weak_ptr<IUiTransformable>());

            virtual ~UiItemBase() = default;

            virtual const glm::ivec2 &GetAbsoluteOrigin() const override;
            virtual size_t GetZOrder() const override;
            virtual size_t GetWidth() const override;
            virtual size_t GetHeight() const override;
            virtual glm::vec2 GetNormalizedTranslation() const override;
            virtual glm::vec2 GetNormalizedScale() const override;
            virtual std::shared_ptr<IUiTransformable> GetRootParent() const override;
            virtual bool IsVisible() const override;
            virtual void SetIsVisible(const bool isVisible) override;

            virtual size_t GetUId() const override;
            std::vector<std::shared_ptr<UiItemBase>> GetAllChildren() const;
            glm::mat4 GetTransformMatrix() const;
            const std::weak_ptr<UiCanvas>& GetParentCanvas() const;

            virtual void SetAbsoluteOrigin(const glm::ivec2 &transform) override;
            virtual void SetZOrder(const size_t z_order) override;
            virtual void SetWidth(const size_t width) override;
            virtual void SetHeight(const size_t height) override;
            void SetUiAnchor(const eUiAnchorType anchorType, const std::shared_ptr<UiItemBase> &anchorUiItem);

            void AddUiItem(const std::shared_ptr<UiItemBase> &uiItem);
            void RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem);
            virtual void RegisterUiItem(const size_t uiId) override;
            virtual void UnregisterUiItem(const size_t uiId) override;
            virtual void UpdateHierarchyTransform();

            virtual std::weak_ptr<::EngineCore::Scene> GetScene() const override;

            virtual void OnRegistered();
            virtual void OnUnregistered();

        protected:
            virtual void OnTransformChanged();

        private:
            void TransformChanged();

            void RebuildTransform();
            void RebuildBoundingArea();
            void RebuildNormalizedTransform(const std::shared_ptr<IUiTransformable> &parent);
            void RebuildTransformMatrix();

            void SyncDataOnRenderThread();
        };
    }
}