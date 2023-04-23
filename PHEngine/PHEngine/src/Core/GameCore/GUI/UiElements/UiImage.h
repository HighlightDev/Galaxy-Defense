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

namespace EngineCore
{
    namespace Scripts
    {
        class LuaProxy;
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

            float mRotationDegrees;

            bool mIsFlipped;

        public:
            UiImage();

            ~UiImage() override;

            void SetTextureSrc(const std::string &textureSrc);

            void SetTexture(const std::shared_ptr<ITexture> &texture);

            std::string GetTextureSrc() const;

            std::shared_ptr<ITexture> GetTexture() const;

            void SetOpacity(const float opacity);

            float GetOpacity() const;

            void SetRotationDegrees(const float rotationDegrees);

            float GetRotationDegrees() const;

            void SetIsFlipped(const bool isFlipped);

            bool GetIsFlipped() const;

            std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const;

            std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

            void OnPropertiesShouldBeUpdatedOnRenderThread() override;

            void OnPropertiesShouldBeUpdatedOnLuaThread() override;

            void SyncFromLuaJsonProperties(const std::string &luaJsonPropsStr) override;

        protected:
            void OnRegistered() override;

            void OnUnregistered() override;

        private:
            void SyncDataOnRenderThread();

            void SyncDataOnLuaThread();

            void ReallocateTexture();
        };
    }
}