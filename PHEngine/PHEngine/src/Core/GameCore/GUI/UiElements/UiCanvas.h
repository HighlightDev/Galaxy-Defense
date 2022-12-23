#pragma once

#include "UiItemBase.h"
#include "IUiTransformable.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/GUI/UiInputSystem/UiInputSystem.h"

#include <unordered_set>
#include <memory>

using namespace Graphics;

namespace Graphics
{
    namespace Proxy
    {
        class UiCanvasSceneProxy;
    }
}

namespace EngineCore
{
    class Scene;

    namespace GUI
    {
        class UiCanvas : public IUiTransformable,
                         public ITickable,
                         public std::enable_shared_from_this<UiCanvas>
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

            std::unique_ptr<UiInputSystem> mInputSystem;

            bool mWasHoveredLastFrame{false}; 

        protected:
            std::vector<std::shared_ptr<UiItemBase>> mChildren;

            std::unordered_set<size_t> mRegisteredUIds;
            std::unordered_set<std::string> mRegisteredNames;

        public:
            explicit UiCanvas(const ViewPortInfo &canvasScreenProperties);

            void InitializeInputSystem();

            virtual size_t GetUId() const override;
            virtual const glm::ivec2 &GetAbsoluteOrigin() const override;
            virtual size_t GetZOrder() const override;
            virtual size_t GetWidth() const override;
            virtual size_t GetHeight() const override;
            virtual glm::vec2 GetNormalizedTranslation() const override;
            virtual glm::vec2 GetNormalizedScale() const override;
            virtual std::weak_ptr<IUiTransformable> GetRootParent() const override;
            virtual std::weak_ptr<IUiTransformable> GetParent() const override;
            virtual std::string GetName() const override;
            virtual bool IsVisible() const override;
            virtual BoundingBox2D GetBoundingArea() const override;
            virtual bool IsTransformDirty() const override;

            virtual void SetAbsoluteOrigin(const glm::ivec2 &transform) override;
            virtual void SetZOrder(const size_t z_order) override;
            virtual void SetWidth(const size_t width) override;
            virtual void SetHeight(const size_t height) override;
            virtual void SetIsVisible(const bool isVisible) override;

            void SetScene(const std::weak_ptr<::EngineCore::Scene> &sceneWp);
            virtual std::weak_ptr<::EngineCore::Scene> GetScene() const override;

            void AddUiItem(const std::shared_ptr<UiItemBase> &uiItem);
            void RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem);

            virtual void Tick(const float deltaTime) override;

            std::shared_ptr<::Graphics::Proxy::UiCanvasSceneProxy> CreateUiCanvasSceneProxy() const;

            std::vector<std::shared_ptr<UiItemBase>> GetDependentByTransformChildren(const std::string& nameOfChangedTransformUiItem) const;

            // Input events
            void OnMousePositionChanged(const glm::ivec2& mouseCursorPosition);
            void OnMouseReleased(const glm::ivec2& mouseCursorPosition);
            void OnMousePressed(const glm::ivec2& mouseCursorPosition);
            void OnMouseClicked(const glm::ivec2& mouseCursorPosition);

        protected:
            void RegisterUiItem(const size_t uiId, const std::string &uiItemName);

            void UnregisterUiItem(const size_t uiId, const std::string &uiItemName);

        private:
            void SyncDataOnRenderThread();

            virtual void SetAnchor(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor, const std::string &dstUiItemName) override;
            virtual void SetAnchorMargin(const eUiAnchor anchor, const int32_t anchorMargin) override;

            virtual std::shared_ptr<IUiTransformable> TryFindChildByName(const std::string &name) const override;

            void SetIsTransformDirty(const bool isDirty);

            void UpdateAnchorTransform();
            void UpdateDependentChildrenAnchorTransform();

            std::vector<std::shared_ptr<UiItemBase>> GetChildrenWithDescendingZOrder() const;
        };
    }
}