#pragma once

#include "UiSceneProxyBase.h"
#include "Core/GameCore/ShaderImplementation/UiTestShader.h"
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
            std::shared_ptr<UiTestShader> mUiTestShader;

            std::shared_ptr<ITexture> mTexture;

            float mOpacity;

        public:
            UiImageSceneProxy(const ::EngineCore::GUI::UiImage* uiImage);

            virtual void Render() override;

            void SetTexture(const std::shared_ptr<ITexture>& texture);

            void SetOpacity(const float opacity);
        };
    }
}