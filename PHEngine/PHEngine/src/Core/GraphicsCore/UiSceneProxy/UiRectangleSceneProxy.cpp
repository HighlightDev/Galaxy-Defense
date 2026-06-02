#include "UiRectangleSceneProxy.h"

#include "Core/GameCore/GUI/UiElements/UiRectangle.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GraphicsCore/PostFX/IPostFxRenderTargetProvider.h"
#include "Core/GraphicsCore/SceneProxy/CameraSceneProxy.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"

#include <gl/glew.h>

using namespace Resources;
using namespace IO;
using namespace EngineCore::GUI;

namespace Graphics {
namespace Proxy {
UiRectangleSceneProxy::UiRectangleSceneProxy(const UiRectangle* uiRectangle)
    : UiSceneProxyBase(uiRectangle)
    , mColor(uiRectangle->GetColor())
    , mOpacity(uiRectangle->GetOpacity())
    , mBorderRadius(uiRectangle->GetBorderRadius())
    , mIsRoundTop(uiRectangle->GetIsRoundTop())
    , mIsRoundBottom(uiRectangle->GetIsRoundBottom())
    , mApplyBlur(uiRectangle->GetApplyBlur())
    , mBlurMix(uiRectangle->GetBlurMix())
{
}

UiRectangleSceneProxy::~UiRectangleSceneProxy()
{
}

void UiRectangleSceneProxy::OnSceneProxyRegistered()
{
    const auto& folderManager = FolderManager::GetInstance();
    ShaderParams shaderParams("UiRectangle Shader");
    shaderParams.SetMainShaders(
        folderManager->GetAbsolutePath("uiVS.glsl"), folderManager->GetAbsolutePath("uiRectangleFS.glsl"));
    mUiRectangleShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiRectangleShader>(shaderParams);
}

void UiRectangleSceneProxy::Render(
    const std::shared_ptr<Graphics::IPostFxRenderTargetProvider>& postFxRenderTargetProvider,
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy)
{
    if (mWidthHeightPixels.x == 0 || mWidthHeightPixels.y == 0) {
        return;
    }

    mUiRectangleShader->ExecuteShader();
    const glm::vec2 scaleOffset = glm::vec2((mNormalizedScale - (mNormalizedScale * mScale)) * 0.5f);
    mUiRectangleShader->SetTransform(mNormalizedTranslation + scaleOffset + mCenterOffset, mNormalizedScale * mScale);
    mUiRectangleShader->SetColor(mColor);
    mUiRectangleShader->SetOpacity(mOpacity * mOverlayOpacity);
    mUiRectangleShader->SetBorderRadius(mBorderRadius);
    mUiRectangleShader->SetWidthHeightPixels(
        glm::vec2(static_cast<float>(mWidthHeightPixels.x), static_cast<float>(mWidthHeightPixels.y)));
    mUiRectangleShader->SetIsRoundTop(mIsRoundTop);
    mUiRectangleShader->SetIsRoundBottom(mIsRoundBottom);

    // Frosted-glass tap. Only enabled when the rectangle asks for it AND the
    // PostFx renderer has a populated Gaussian blur RT this frame; otherwise
    // we leave the body colour untouched. The shader's sampler still has to be
    // bound to slot 0 to avoid an "incomplete texture" warning, so we bind the
    // blur RT when it's available regardless of the apply flag.
    bool blurActive = false;
    if (postFxRenderTargetProvider) {
        const auto& blurTexture = postFxRenderTargetProvider->GetRenderTargetTextureByKey(ePostFxStageType::GAUSSIAN_BLUR_STAGE);
        if (blurTexture) {
            blurTexture->BindTexture(0);
            mUiRectangleShader->SetBlurSampler(0);
            blurActive = mApplyBlur;
        }
    }
    const auto& viewPortInfo = cameraSceneProxy->GetViewPort();
    const auto screenResolution = glm::vec2(viewPortInfo.Width, viewPortInfo.Height);

    mUiRectangleShader->SetApplyBlur(blurActive);
    mUiRectangleShader->SetBlurMix(mBlurMix);
    mUiRectangleShader->SetScreenResolution(screenResolution);

    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLE_STRIP);
    mUiRectangleShader->StopShader();
}

void UiRectangleSceneProxy::SetColor(const glm::vec3& color)
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

void UiRectangleSceneProxy::SetIsRoundTop(const bool bIsRoundTop)
{
    mIsRoundTop = bIsRoundTop;
}

void UiRectangleSceneProxy::SetIsRoundBottom(const bool bIsRoundBottom)
{
    mIsRoundBottom = bIsRoundBottom;
}

void UiRectangleSceneProxy::SetApplyBlur(const bool applyBlur)
{
    mApplyBlur = applyBlur;
}

void UiRectangleSceneProxy::SetBlurMix(const float blurMix)
{
    mBlurMix = blurMix;
}

void UiRectangleSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mUiRectangleShader);
}
} // namespace Proxy
} // namespace Graphics