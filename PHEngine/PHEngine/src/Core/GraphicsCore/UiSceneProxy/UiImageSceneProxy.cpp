#include "UiImageSceneProxy.h"

#include "Core/GameCore/GUI/UiElements/UiImage.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/UtilityCore/EngineMath.h"

#include <gl/glew.h>

using namespace Resources;
using namespace IO;
using namespace Graphics;
using namespace EngineCore::GUI;

namespace Graphics {
namespace Proxy {
UiImageSceneProxy::UiImageSceneProxy(const UiImage* uiImage)
    : UiSceneProxyBase(uiImage)
    , mTexture(uiImage->GetTexture())
    , mOpacity(uiImage->GetOpacity())
    , mRotationDegrees(uiImage->GetRotationDegrees())
    , mIsFlipped(false)
{
}

UiImageSceneProxy::~UiImageSceneProxy()
{
}

void UiImageSceneProxy::Render()
{
    if (mTexture) {
        mUiImageShader->ExecuteShader();
        const glm::vec2 scaleOffset = glm::vec2((mNormalizedScale - (mNormalizedScale * mScale)) * 0.5f);
        mUiImageShader->SetTransform(mNormalizedTranslation + mCenterOffset + scaleOffset, mNormalizedScale * mScale);

        mTexture->BindTexture(0);
        mUiImageShader->SetImageTexture(0);
        mUiImageShader->SetIsCustomColorEnabled(mIsCustomColor);
        mUiImageShader->SetCustomColor(mColor);
        mUiImageShader->SetOpacity(mOpacity * mOverlayOpacity);
        mUiImageShader->SetRotationRadians(glm::radians<float>(mRotationDegrees));
        mUiImageShader->SetIsFlipped(mIsFlipped);
        ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
        mUiImageShader->StopShader();
    }
}

void UiImageSceneProxy::SetTexture(const std::shared_ptr<ITexture>& texture)
{
    mTexture = texture;
}

void UiImageSceneProxy::SetUseCustomColor(const bool isCustomColorEnabled)
{
    mIsCustomColor = isCustomColorEnabled;
}

void UiImageSceneProxy::SetColor(const glm::vec3& color)
{
    mColor = color;
}

void UiImageSceneProxy::SetOpacity(const float opacity)
{
    mOpacity = opacity;
}

void UiImageSceneProxy::SetRotationDegrees(const float rotationDegrees)
{
    mRotationDegrees = rotationDegrees;
}

void UiImageSceneProxy::SetIsFlipped(const bool isFlipped)
{
    mIsFlipped = isFlipped;
}

void UiImageSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mUiImageShader);
}

void UiImageSceneProxy::OnSceneProxyRegistered()
{
    const auto& folderManager = FolderManager::GetInstance();
    ShaderParams shaderParams(
        "UiImage Shader",
        folderManager->GetShadersPath() + "uiVS.glsl",
        folderManager->GetShadersPath() + "uiImageFS.glsl",
        "",
        "",
        "",
        "");
    mUiImageShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiImageShader>(shaderParams);
}
} // namespace Proxy
} // namespace Graphics