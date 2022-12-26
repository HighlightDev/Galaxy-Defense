#pragma once

#include "UiSceneProxyBase.h"
#include "Core/GameCore/ShaderImplementation/UiRectangleShader.h"

#include <glm/vec4.hpp>

namespace EngineCore
{
    namespace GUI
    {
        class UiRectangle;
    }
}

using namespace EngineCore::ShaderImpl;

namespace Graphics
{
    namespace Proxy
    {
        class UiRectangleSceneProxy : public UiSceneProxyBase
        {
            std::shared_ptr<UiRectangleShader> mUiRectangleShader;

            glm::vec4 mColor;

        public:
            UiRectangleSceneProxy(const ::EngineCore::GUI::UiRectangle* uiRectangle);

            ~UiRectangleSceneProxy() override;

            void Render() override;

            void SetColor(const glm::vec4& color);

            void CleanUp() override;
        };
    }
}