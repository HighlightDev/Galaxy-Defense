#pragma once

#include "UiItemBase.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

namespace Graphics
{
    namespace Proxy
    {
        class UiSceneProxyBase;
    }
}

using namespace Graphics::Texture;

namespace EngineCore
{
    class UiCanvas;
    namespace GUI
    {
        class UiImage : public UiItemBase
        {
            std::string mTextureSrc;
            std::shared_ptr<ITexture> mTexture;

            float mOpacity;

        public:
            explicit UiImage(const std::weak_ptr<UiCanvas> &canvasParent, const std::weak_ptr<IUiTransformable> &parent);

            ~UiImage() override;

            void SetTextureSrc(const std::string &textureSrc);

            void SetTexture(const std::shared_ptr<ITexture>& texture);

            std::string GetTextureSrc() const;

            std::shared_ptr<ITexture> GetTexture() const;

            void SetOpacity(const float opacity);

            float GetOpacity() const;

            std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const;

        protected:
            virtual void OnRegistered() override;

            virtual void OnUnregistered() override;

        private:
            void SyncDataOnRenderThread();

            void ReallocateTexture();
        };
    }
}