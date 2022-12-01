#pragma once

#include "UiItemBase.h"
#include "IUiTransformable.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

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
        class UiCanvas : public IUiTransformable
        {
        private:
            static size_t s_UId;
            size_t mUId;

            std::weak_ptr<::EngineCore::Scene> mScene;

            glm::ivec2 mAbsoluteOrigin;
            glm::ivec2 mWidthHeight;
            bool mIsVisible;

        protected:
            std::vector<std::shared_ptr<UiItemBase>> mChildren;
            std::unordered_set<size_t> mRegisteredUiItems;

        public:
            explicit UiCanvas(const ViewPortInfo &canvasScreenProperties);

            virtual size_t GetUId() const override;
            virtual const glm::ivec2 &GetAbsoluteOrigin() const override;
            virtual size_t GetZOrder() const override;
            virtual size_t GetWidth() const override;
            virtual size_t GetHeight() const override;
            virtual glm::vec2 GetNormalizedTranslation() const override;
            virtual glm::vec2 GetNormalizedScale() const override;
            virtual std::shared_ptr<IUiTransformable> GetRootParent() const override;
            virtual bool IsVisible() const override;

            virtual void SetAbsoluteOrigin(const glm::ivec2 &transform) override;
            virtual void SetZOrder(const size_t z_order) override;
            virtual void SetWidth(const size_t width) override;
            virtual void SetHeight(const size_t height) override;
            virtual void SetIsVisible(const bool isVisible) override;

            void SetScene(const std::weak_ptr<::EngineCore::Scene>& sceneWp);
            virtual std::weak_ptr<::EngineCore::Scene> GetScene() const override;

            void AddUiItem(const std::shared_ptr<UiItemBase> &uiItem);
            void RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem);

            std::shared_ptr<::Graphics::Proxy::UiCanvasSceneProxy> CreateUiCanvasSceneProxy() const;

        private:
            void UpdateHierarchyTransform();

            void SyncDataOnRenderThread();

        protected:
            void RegisterUiItem(const size_t uiId);
            void UnregisterUiItem(const size_t uiId);
        };
    }
}