#pragma once

#include "UiSceneProxyBase.h"
#include "Core/GameCore/ShaderImplementation/FontRenderingShader.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

namespace EngineCore
{
    namespace GUI
    {
        class UiLabel;
    }
}

using namespace EngineCore::ShaderImpl;
using namespace Graphics::Texture;

namespace Graphics
{
    class TextFieldProxy;

    namespace Proxy
    {
        class UiLabelSceneProxy : public UiSceneProxyBase
        {
            std::shared_ptr<FontRenderingShader> mUiLabelShader;

            std::string mText;

            std::shared_ptr<ITexture> mFontTexture;

            float mOpacity;

            std::shared_ptr<::Graphics::TextFieldProxy> mTextFieldProxy;

        public:
            UiLabelSceneProxy(const ::EngineCore::GUI::UiLabel *uiLabel);

            ~UiLabelSceneProxy() override;

            void Render() override;

            void SetText(const std::string &text);

            void SetOpacity(const float opacity);

            void CleanUp() override;

            void OnSceneProxyRegistered() override;

        private:

            void Initialize();
        };
    }
}