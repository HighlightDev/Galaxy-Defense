#include "UiUpgradeIconSceneProxy.h"

#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Implementation/GUI/UiUpgradeIcon.h"

#include <gl/glew.h>

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
}

void UiUpgradeIconSceneProxy::Render()
{
    if (mWidthHeightPixels.x == 0 || mWidthHeightPixels.y == 0) {
        return;
    }

    mShader->ExecuteShader();
    const glm::vec2 scaleOffset = glm::vec2((mNormalizedScale - (mNormalizedScale * mScale)) * 0.5f);
    mShader->SetTransform(mNormalizedTranslation + mCenterOffset + scaleOffset, mNormalizedScale * mScale);
    mShader->SetRotationRadians(glm::radians<float>(mRotationDegrees));
    mShader->SetIsFlipped(mIsFlipped);
    mShader->SetWidthHeightPixels(glm::vec2(static_cast<float>(mWidthHeightPixels.x), static_cast<float>(mWidthHeightPixels.y)));
    mShader->SetFillColor(mFillColor);
    mShader->SetBorderColor(mBorderColor);
    mShader->SetGlowColor(mGlowColor);
    mShader->SetIconCustomColor(mIconCustomColor);
    mShader->SetIsCustomIconColor(mIsCustomIconColor);
    mShader->SetFillStrength(mFillStrength);
    mShader->SetBorderThicknessPx(mBorderThicknessPx);
    mShader->SetGlowSizePx(mGlowSizePx);
    mShader->SetOpacity(mOpacity * mOverlayOpacity);

    if (mTexture) {
        mTexture->BindTexture(0);
        mShader->SetImageTexture(0);
    } else {
        mShader->SetImageTexture(0);
    }

    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
    mShader->StopShader();
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

void UiUpgradeIconSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mShader);
}

} // namespace Game
