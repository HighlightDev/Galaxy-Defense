#pragma once

#include "UiSceneProxyBase.h"
#include "Core/GameCore/ShaderImplementation/UiRectangleShader.h"

#include <glm/vec3.hpp>

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

            glm::vec3 mColor;

            float mOpacity;

            float mBorderRadius;

        public:
            UiRectangleSceneProxy(const ::EngineCore::GUI::UiRectangle* uiRectangle);

            ~UiRectangleSceneProxy() override;

            void Render() override;

            void SetColor(const glm::vec3& color);

            void SetOpacity(const float opacity);

            void SetBorderRadius(const float borderRadiusPx);

            void CleanUp() override;

            void OnSceneProxyRegistered() override;
        };
    }
}