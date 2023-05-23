#include "UiRectangleSceneProxy.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GameCore/GUI/UiElements/UiRectangle.h"

#include <gl/glew.h>

using namespace Resources;
using namespace IO;
using namespace EngineCore::GUI;

namespace Graphics
{
    namespace Proxy
    {
        UiRectangleSceneProxy::UiRectangleSceneProxy(const UiRectangle *uiRectangle)
            : UiSceneProxyBase(uiRectangle),
              mColor(uiRectangle->GetColor()),
              mOpacity(uiRectangle->GetOpacity()),
              mBorderRadius(uiRectangle->GetBorderRadius())
        {
        }

        UiRectangleSceneProxy::~UiRectangleSceneProxy()
        {
            CleanUp();
        }

        void UiRectangleSceneProxy::OnSceneProxyRegistered()
        {
            const auto &folderManager = FolderManager::GetInstance();
            ShaderParams shaderParams("UiRectangle Shader", folderManager->GetShadersPath() + "uiVS.glsl", folderManager->GetShadersPath() + "uiRectangleFS.glsl", "", "", "", "");
            mUiRectangleShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiRectangleShader>(shaderParams);
        }

        void UiRectangleSceneProxy::Render()
        {
            mUiRectangleShader->ExecuteShader();
            mUiRectangleShader->SetTransform(mNormalizedTranslation, mNormalizedScale);
            mUiRectangleShader->SetColor(mColor);
            mUiRectangleShader->SetOpacity(mOpacity * mOverlayOpacity);
            mUiRectangleShader->SetBorderRadius(mBorderRadius);
            mUiRectangleShader->SetWidthHeightPixels(glm::vec2(static_cast<float>(mWidthHightPixels.x), static_cast<float>(mWidthHightPixels.y)));
            ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
            mUiRectangleShader->StopShader();
        }

        void UiRectangleSceneProxy::SetColor(const glm::vec3 &color)
        {
            mColor = color;
        }

        void UiRectangleSceneProxy::SetOpacity(const float opacity)
        {
            mOpacity = opacity;
        }

        void UiRectangleSceneProxy::SetBorderRadius(const float borderRadiusPx)
        {
            mBorderRadius = borderRadiusPx;
        }

        void UiRectangleSceneProxy::CleanUp()
        {
            ShaderPool::GetInstance()->TryToFreeMemory(mUiRectangleShader);
        }
    }
}