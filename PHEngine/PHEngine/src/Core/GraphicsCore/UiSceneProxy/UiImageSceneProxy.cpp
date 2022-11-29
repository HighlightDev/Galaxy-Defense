#include "UiImageSceneProxy.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GameCore/GUI/UiElements/UiImage.h"

#include <gl/glew.h>

using namespace Resources;
using namespace IO;
using namespace Graphics;
using namespace EngineCore::GUI;

namespace Graphics
{
    namespace Proxy
    {
        UiImageSceneProxy::UiImageSceneProxy(const UiImage* uiImage)
            : UiSceneProxyBase(uiImage)
        {
            const auto &folderManager = FolderManager::GetInstance();
            ShaderParams shaderParams("UiTest Shader", folderManager->GetShadersPath() + "uiTestVS.glsl", folderManager->GetShadersPath() + "uiTestFS.glsl", "", "", "", "");
            mUiTestShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiTestShader>(shaderParams);
        }

        void UiImageSceneProxy::Render()
        {
            const auto &transformMatrix = GetTransformMatrix();
            mUiTestShader->ExecuteShader();
            mUiTestShader->SetTransformMatrix(transformMatrix);
            glm::vec4 color = glm::vec4(1.0);
            mUiTestShader->SetColor(color);
            ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
            mUiTestShader->StopShader();
        }
    }
}