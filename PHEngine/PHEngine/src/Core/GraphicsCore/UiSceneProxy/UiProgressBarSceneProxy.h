#pragma once

#include "UiSceneProxyBase.h"
#include "Core/GameCore/ShaderImplementation/UiProgressBarShader.h"

#include <glm/vec3.hpp>

namespace EngineCore
{
    namespace GUI
    {
        class UiProgressBar;
    }
}

using namespace EngineCore::ShaderImpl;

namespace Graphics
{
    namespace Proxy
    {
        class UiProgressBarSceneProxy : public UiSceneProxyBase
        {
            std::shared_ptr<UiProgressBarShader> mUiProgressBarShader;

            glm::vec3 mEmptyColor;

            glm::vec3 mFilledColor;

            float mOpacity;

            float mFillPercentValue;

        public:
            UiProgressBarSceneProxy(const ::EngineCore::GUI::UiProgressBar *uiProgressBar);

            ~UiProgressBarSceneProxy() override;

            void Render() override;

            void SetEmptyColor(const glm::vec3 &color);

            void SetFilledColor(const glm::vec3 &color);

            void SetOpacity(const float opacity);

            void SetFillPercentValue(const float fillPercentValue);

            void CleanUp() override;

            void OnSceneProxyRegistered() override;
        };
    }
}