#pragma once

#include "UiItemBase.h"

namespace Graphics
{
    namespace Proxy
    {
        class UiSceneProxyBase;
    }
}

namespace EngineCore
{
    class UiCanvas;
    namespace GUI
    {
        class UiImage : public UiItemBase
        {

            glm::vec4 mColor;

        public:
            explicit UiImage(const std::weak_ptr<UiCanvas>& canvasParent, const std::weak_ptr<IUiTransformable> &parent = std::weak_ptr<IUiTransformable>());

            ~UiImage() override = default;

            void SetColor(const glm::vec4 &color);

            glm::vec4 GetColor() const;

            std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const;

        protected:
            virtual void UpdateHierarchyTransform() override;

            virtual void OnRegistered() override;

            virtual void OnDeregistered() override;
        };
    }
}