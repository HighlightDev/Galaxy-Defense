#include "UiToggleButtonSceneProxy.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GameCore/GUI/UiElements/UiToggleButton.h"

#include <gl/glew.h>

using namespace Resources;
using namespace IO;
using namespace EngineCore::GUI;

namespace Graphics
{
    namespace Proxy
    {
        UiToggleButtonSceneProxy::UiToggleButtonSceneProxy(const UiToggleButton *uiToggleButton)
            : UiSceneProxyBase(uiToggleButton),
              mToggleOnColor(uiToggleButton->GetToggleOnColor()),
              mToggleOffColor(uiToggleButton->GetToggleOffColor()),
              mIsStateOn(uiToggleButton->IsButtonStateOn()),
              mOpacity(uiToggleButton->GetOpacity())
        {
        }

        UiToggleButtonSceneProxy::~UiToggleButtonSceneProxy()
        {
            CleanUp();
        }

        void UiToggleButtonSceneProxy::OnSceneProxyRegistered()
        {
            const auto &folderManager = FolderManager::GetInstance();
            ShaderParams shaderParams("UiRectangle Shader", folderManager->GetShadersPath() + "uiVS.glsl", folderManager->GetShadersPath() + "uiRectangleFS.glsl", "", "", "", "");
            mUiRectangleShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiRectangleShader>(shaderParams);
        }

        void UiToggleButtonSceneProxy::Render()
        {
            mUiRectangleShader->ExecuteShader();
            // Render background
            mUiRectangleShader->SetTransform(mNormalizedTranslation, mNormalizedScale);
            mUiRectangleShader->SetColor(mToggleOffColor);
            mUiRectangleShader->SetOpacity(mOpacity * mOverlayOpacity);
            mUiRectangleShader->SetWidthHeightPixels(mWidthHightPixels);
            mUiRectangleShader->SetBorderRadius(0.0f);
            ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);

            if (mIsStateOn)
            {
                // Render foreground
                static constexpr float normalizedToggleOnRectangleSize = 0.7f;
                static const glm::vec2 toggleOnRectangleCenteredOffset = glm::vec2((mNormalizedScale - (mNormalizedScale * normalizedToggleOnRectangleSize)) * 0.5f);
                mUiRectangleShader->SetTransform(mNormalizedTranslation + toggleOnRectangleCenteredOffset, mNormalizedScale * glm::vec2(normalizedToggleOnRectangleSize));
                mUiRectangleShader->SetColor(mToggleOnColor);
                ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
            }
            mUiRectangleShader->StopShader();
        }

        void UiToggleButtonSceneProxy::SetToggleOnColor(const glm::vec3 &color)
        {
            mToggleOnColor = color;
        }

        void UiToggleButtonSceneProxy::SetToggleOffColor(const glm::vec3 &color)
        {
            mToggleOffColor = color;
        }

        void UiToggleButtonSceneProxy::SetOpacity(const float opacity)
        {
            mOpacity = opacity;
        }

        void UiToggleButtonSceneProxy::SetState(const bool state)
        {
            mIsStateOn = state;
        }

        void UiToggleButtonSceneProxy::CleanUp()
        {
            ShaderPool::GetInstance()->TryToFreeMemory(mUiRectangleShader);
        }
    }
}