#include "UiUpgradeIconSceneProxy.h"

#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Implementation/GUI/UiUpgradeIcon.h"

#include <gl/glew.h>

#undef max
#undef min

using namespace Resources;
using namespace IO;
using namespace Graphics;
using namespace EngineCore::GUI;

namespace Game {

UiUpgradeIconSceneProxy::UiUpgradeIconSceneProxy(const UiUpgradeIcon* uiUpgradeIcon)
    : UiSceneProxyBase(uiUpgradeIcon)
    , mTexture(uiUpgradeIcon->GetTexture())
    , mFillColor(uiUpgradeIcon->GetFillColor())
    , mBorderColor(uiUpgradeIcon->GetBorderColor())
    , mGlowColor(uiUpgradeIcon->GetGlowColor())
    , mIconCustomColor(uiUpgradeIcon->GetTextureColor())
    , mIsCustomIconColor(uiUpgradeIcon->IsCustomColorEnabled())
    , mFillStrength(uiUpgradeIcon->GetFillStrength())
    , mBorderThicknessPx(uiUpgradeIcon->GetBorderThicknessPx())
    , mGlowSizePx(uiUpgradeIcon->GetGlowSizePx())
    , mOpacity(uiUpgradeIcon->GetOpacity())
    , mRotationDegrees(uiUpgradeIcon->GetRotationDegrees())
    , mIsFlipped(uiUpgradeIcon->GetIsFlipped())
    , mGlowVisible(uiUpgradeIcon->GetGlowVisible())
{
}

UiUpgradeIconSceneProxy::~UiUpgradeIconSceneProxy()
{
}

void UiUpgradeIconSceneProxy::OnSceneProxyRegistered()
{
    const auto& folderManager = FolderManager::GetInstance();
    ShaderParams shaderParams("UiUpgradeIcon Shader");
    shaderParams.SetMainShaders(
        folderManager->GetAbsolutePath("uiVS.glsl"), folderManager->GetAbsolutePath("uiUpgradeIconFS.glsl"));
    mShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiUpgradeIconShader>(shaderParams);

    ShaderParams glowShaderParams("UiUpgradeIconGlow Shader");
    glowShaderParams.SetMainShaders(
        folderManager->GetAbsolutePath("uiVS.glsl"), folderManager->GetAbsolutePath("uiUpgradeIconGlowFS.glsl"));
    mGlowShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiUpgradeIconGlowShader>(glowShaderParams);
}

void UiUpgradeIconSceneProxy::Render(
    const std::shared_ptr<Graphics::IPostFxRenderTargetProvider>& postFxRenderTargetProvider,
    const std::shared_ptr<Graphics::CameraSceneProxy>& cameraSceneProxy)
{
    if (mWidthHeightPixels.x == 0 || mWidthHeightPixels.y == 0) {
        return;
    }

    const glm::vec2 scaleOffset = glm::vec2((mNormalizedScale - (mNormalizedScale * mScale)) * 0.5f);
    const glm::vec2 iconTranslation = mNormalizedTranslation + mCenterOffset + scaleOffset;
    const glm::vec2 iconScale = mNormalizedScale * mScale;
    const glm::vec2 iconPixels = glm::vec2(static_cast<float>(mWidthHeightPixels.x), static_cast<float>(mWidthHeightPixels.y));
    const float renderOpacity = mOpacity * mOverlayOpacity;

    mShader->ExecuteShader();
    mShader->SetTransform(iconTranslation, iconScale);
    mShader->SetRotationRadians(glm::radians<float>(mRotationDegrees));
    mShader->SetIsFlipped(mIsFlipped);
    mShader->SetWidthHeightPixels(iconPixels);
    mShader->SetFillColor(mFillColor);
    mShader->SetBorderColor(mBorderColor);
    mShader->SetIconCustomColor(mIconCustomColor);
    mShader->SetIsCustomIconColor(mIsCustomIconColor);
    mShader->SetFillStrength(mFillStrength);
    mShader->SetBorderThicknessPx(mBorderThicknessPx);
    mShader->SetOpacity(renderOpacity);

    if (mTexture) {
        mTexture->BindTexture(0);
        mShader->SetImageTexture(0);
    } else {
        mShader->SetImageTexture(0);
    }

    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLE_STRIP);
    mShader->StopShader();

    if (mGlowVisible && mGlowSizePx > 0.0f) {
        const float padPx = mGlowSizePx + 2.0f;
        const glm::vec2 normPerPx = iconScale / glm::max(iconPixels, glm::vec2(1.0f));
        const glm::vec2 glowScale = iconScale + 2.0f * padPx * normPerPx;
        const glm::vec2 glowTranslation = iconTranslation - padPx * normPerPx;
        const glm::vec2 glowPixels = iconPixels + 2.0f * padPx;

        mGlowShader->ExecuteShader();
        mGlowShader->SetTransform(glowTranslation, glowScale);
        mGlowShader->SetWidthHeightPixels(glowPixels);
        mGlowShader->SetIconWidthHeightPixels(iconPixels);
        mGlowShader->SetGlowColor(mGlowColor);
        mGlowShader->SetGlowSizePx(mGlowSizePx);
        mGlowShader->SetOpacity(renderOpacity);

        ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLE_STRIP);
        mGlowShader->StopShader();
    }
}

void UiUpgradeIconSceneProxy::SetTexture(const std::shared_ptr<ITexture>& texture)
{
    mTexture = texture;
}

void UiUpgradeIconSceneProxy::SetFillColor(const glm::vec3& color)
{
    mFillColor = color;
}

void UiUpgradeIconSceneProxy::SetBorderColor(const glm::vec3& color)
{
    mBorderColor = color;
}

void UiUpgradeIconSceneProxy::SetGlowColor(const glm::vec3& color)
{
    mGlowColor = color;
}

void UiUpgradeIconSceneProxy::SetIconCustomColor(const glm::vec3& color)
{
    mIconCustomColor = color;
}

void UiUpgradeIconSceneProxy::SetIsCustomIconColor(const bool isCustomIconColor)
{
    mIsCustomIconColor = isCustomIconColor;
}

void UiUpgradeIconSceneProxy::SetFillStrength(const float fillStrength)
{
    mFillStrength = fillStrength;
}

void UiUpgradeIconSceneProxy::SetBorderThicknessPx(const float borderThicknessPx)
{
    mBorderThicknessPx = borderThicknessPx;
}

void UiUpgradeIconSceneProxy::SetGlowSizePx(const float glowSizePx)
{
    mGlowSizePx = glowSizePx;
}

void UiUpgradeIconSceneProxy::SetOpacity(const float opacity)
{
    mOpacity = opacity;
}

void UiUpgradeIconSceneProxy::SetRotationDegrees(const float rotationDegrees)
{
    mRotationDegrees = rotationDegrees;
}

void UiUpgradeIconSceneProxy::SetIsFlipped(const bool isFlipped)
{
    mIsFlipped = isFlipped;
}

void UiUpgradeIconSceneProxy::SetGlowVisible(const bool glowVisible)
{
    mGlowVisible = glowVisible;
}

void UiUpgradeIconSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mShader);
    ShaderPool::GetInstance()->TryToFreeMemory(mGlowShader);
}

} // namespace Game
