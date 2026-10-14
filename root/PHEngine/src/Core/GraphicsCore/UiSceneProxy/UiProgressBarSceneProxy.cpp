#include "UiProgressBarSceneProxy.h"

#include "Core/GameCore/GUI/UiElements/UiProgressBar.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"

#include <gl/glew.h>

using namespace Resources;
using namespace IO;
using namespace EngineCore::GUI;

namespace Graphics {
namespace Proxy {

UiProgressBarSceneProxy::UiProgressBarSceneProxy(const UiProgressBar* uiProgressBar)
    : UiSceneProxyBase(uiProgressBar)
    , mEmptyColor(uiProgressBar->GetEmptyColor())
    , mFilledColor(uiProgressBar->GetFilledColor())
    , mOpacity(uiProgressBar->GetOpacity())
    , mFillPercentValue(uiProgressBar->GetFillPercentValue())
{
}

UiProgressBarSceneProxy::~UiProgressBarSceneProxy()
{
}

void UiProgressBarSceneProxy::OnSceneProxyRegistered()
{
    const auto& folderManager = FolderManager::GetInstance();
    ShaderParams shaderParams("UiProgressBar Shader");
    shaderParams.SetMainShaders(
        folderManager->GetAbsolutePathToRes("uiVS.glsl"), folderManager->GetAbsolutePathToRes("uiProgressBarFS.glsl"));
    mUiProgressBarShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiProgressBarShader>(shaderParams);
}

void UiProgressBarSceneProxy::Render(
    const std::shared_ptr<Graphics::IPostFxRenderTargetProvider>& postFxRenderTargetProvider,
    const std::shared_ptr<Graphics::CameraSceneProxy>& cameraSceneProxy)
{
    mUiProgressBarShader->ExecuteShader();
    const glm::vec2 scaleOffset = glm::vec2((mNormalizedScale - (mNormalizedScale * mScale)) * 0.5f);
    mUiProgressBarShader->SetTransform(mNormalizedTranslation + scaleOffset + mCenterOffset, mNormalizedScale * mScale);
    mUiProgressBarShader->SetEmptyColor(mEmptyColor);
    mUiProgressBarShader->SetFilledColor(mFilledColor);
    mUiProgressBarShader->SetFillPercentValue(mFillPercentValue);
    mUiProgressBarShader->SetOpacity(mOpacity * mOverlayOpacity);
    mUiProgressBarShader->SetBorderRadius(mBorderRadius);
    mUiProgressBarShader->SetWidthHeightPixels(
        glm::vec2(static_cast<float>(mWidthHeightPixels.x), static_cast<float>(mWidthHeightPixels.y)));
    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLE_STRIP);
    mUiProgressBarShader->StopShader();
}

void UiProgressBarSceneProxy::SetEmptyColor(const glm::vec3& color)
{
    mEmptyColor = color;
}

void UiProgressBarSceneProxy::SetFilledColor(const glm::vec3& color)
{
    mFilledColor = color;
}

void UiProgressBarSceneProxy::SetOpacity(const float opacity)
{
    mOpacity = opacity;
}

void UiProgressBarSceneProxy::SetFillPercentValue(const float fillValue)
{
    mFillPercentValue = fillValue;
}

void UiProgressBarSceneProxy::SetBorderRadius(const float borderRadius)
{
    mBorderRadius = borderRadius;
}

void UiProgressBarSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mUiProgressBarShader);
}
} // namespace Proxy
} // namespace Graphics