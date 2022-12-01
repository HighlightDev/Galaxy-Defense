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
        UiImageSceneProxy::UiImageSceneProxy(const UiImage *uiImage)
            : UiSceneProxyBase(uiImage),
              mTexture(uiImage->GetTexture()),
              mOpacity(uiImage->GetOpacity())
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
            if (mTexture)
            {
                mTexture->BindTexture(0);
                mUiTestShader->SetImageTexture(0);
            }
            mUiTestShader->SetOpacity(mOpacity);
            ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
            mUiTestShader->StopShader();
        }

        void UiImageSceneProxy::SetTexture(const std::shared_ptr<ITexture> &texture)
        {
            mTexture = texture;
        }

        void UiImageSceneProxy::SetOpacity(const float opacity)
        {
            mOpacity = opacity;
        }
    }
}