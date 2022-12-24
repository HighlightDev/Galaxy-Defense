#pragma once

#include "UiSceneProxyBase.h"
#include "Core/GameCore/ShaderImplementation/UiImageShader.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

namespace EngineCore
{
    namespace GUI
    {
        class UiImage;
    }
}

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Texture;

namespace Graphics
{
    namespace Proxy
    {
        class UiImageSceneProxy : public UiSceneProxyBase
        {
            std::shared_ptr<UiImageShader> mUiImageShader;

            std::shared_ptr<ITexture> mTexture;

            float mOpacity;

        public:
            UiImageSceneProxy(const ::EngineCore::GUI::UiImage* uiImage);

            ~UiImageSceneProxy() override;

            virtual void Render() override;

            void SetTexture(const std::shared_ptr<ITexture>& texture);

            void SetOpacity(const float opacity);

            virtual void CleanUp() override;
        };
    }
}