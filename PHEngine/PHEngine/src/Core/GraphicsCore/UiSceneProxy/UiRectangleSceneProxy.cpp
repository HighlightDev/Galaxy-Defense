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
              mColor(uiRectangle->GetColor())
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
            ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
            mUiRectangleShader->StopShader();
        }

        void UiRectangleSceneProxy::SetColor(const glm::vec4 &color)
        {
            mColor = color;
        }

        void UiRectangleSceneProxy::CleanUp()
        {
            ShaderPool::GetInstance()->TryToFreeMemory(mUiRectangleShader);
        }
    }
}