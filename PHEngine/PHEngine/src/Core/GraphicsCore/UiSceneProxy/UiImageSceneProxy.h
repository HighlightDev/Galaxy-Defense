#pragma once

#include "UiSceneProxyBase.h"
#include "Core/GameCore/ShaderImplementation/UiImageShader.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

#include <glm/vec3.hpp>

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

            bool mIsCustomColor;

            glm::vec3 mColor;

            float mRotationDegrees;

            bool mIsFlipped;

        public:
            UiImageSceneProxy(const ::EngineCore::GUI::UiImage* uiImage);

            ~UiImageSceneProxy() override;

            void Render() override;

            void SetTexture(const std::shared_ptr<ITexture>& texture);

            void SetUseCustomColor(const bool isCustomColorEnabled);

            void SetColor(const glm::vec3& color);

            void SetOpacity(const float opacity);

            void SetRotationDegrees(const float rotationDegrees);

            void SetIsFlipped(const bool isFlipped);

            void CleanUp() override;

            void OnSceneProxyRegistered() override;
        };
    }
}