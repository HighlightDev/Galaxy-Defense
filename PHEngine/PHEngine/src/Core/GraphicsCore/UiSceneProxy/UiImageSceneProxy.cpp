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
            ShaderParams shaderParams("UiImage Shader", folderManager->GetShadersPath() + "uiVS.glsl", folderManager->GetShadersPath() + "uiImageFS.glsl", "", "", "", "");
            mUiImageShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiImageShader>(shaderParams);
        }

        UiImageSceneProxy::~UiImageSceneProxy()
        {
            CleanUp();
        }

        void UiImageSceneProxy::Render()
        {
            mUiImageShader->ExecuteShader();
            mUiImageShader->SetTransform(mNormalizedTranslation, mNormalizedScale);
            if (mTexture)
            {
                mTexture->BindTexture(0);
                mUiImageShader->SetImageTexture(0);
            }
            mUiImageShader->SetOpacity(mOpacity);
            ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
            mUiImageShader->StopShader();
        }

        void UiImageSceneProxy::SetTexture(const std::shared_ptr<ITexture> &texture)
        {
            mTexture = texture;
        }

        void UiImageSceneProxy::SetOpacity(const float opacity)
        {
            mOpacity = opacity;
        }

        void UiImageSceneProxy::CleanUp()
        {
            ShaderPool::GetInstance()->TryToFreeMemory(mUiImageShader);
        }
    }
}