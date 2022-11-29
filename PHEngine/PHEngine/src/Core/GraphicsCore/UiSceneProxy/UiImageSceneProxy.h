#pragma once

#include "UiSceneProxyBase.h"
#include "Core/GameCore/ShaderImplementation/UiTestShader.h"

namespace EngineCore
{
    namespace GUI
    {
        class UiImage;
    }
}

using namespace EngineCore::ShaderImpl;

namespace Graphics
{
    namespace Proxy
    {
        class UiImageSceneProxy : public UiSceneProxyBase
        {
            std::shared_ptr<UiTestShader> mUiTestShader;

        public:
            UiImageSceneProxy(const ::EngineCore::GUI::UiImage* uiImage);

            virtual void Render() override;
        };
    }
}