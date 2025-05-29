#include "UiSliderSceneProxy.h"

#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GraphicsCore/UiSceneProxy/UiCanvasSceneProxy.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"

#include <gl/glew.h>

using namespace Resources;
using namespace IO;

namespace Graphics::Proxy {
UiSliderSceneProxy::UiSliderSceneProxy(const UiSlider* uiSliderBar)
    : UiSceneProxyBase(uiSliderBar)
    , mMaxSliderValue(uiSliderBar->GetMaxSliderValue())
    , mMinSliderValue(uiSliderBar->GetMinSliderValue())
    , mSliderValue(uiSliderBar->GetSliderValue())
    , mSliderStep(uiSliderBar->GetSliderStep())
    , mSliderThicknessPixels(uiSliderBar->GetSliderThicknessPixels())
    , mBlobThicknessPixels(uiSliderBar->GetBlobThicknessPixels())
    , mSliderType(uiSliderBar->GetSliderType())
    , mSliderToCenterOffset(uiSliderBar->GetSliderToCenterOffset())
    , mSliderThicknessScale(uiSliderBar->GetSliderThicknessScale())
    , mBlobThicknessScale(uiSliderBar->GetBlobThicknessScale())
    , mBlobToCenterOffset(uiSliderBar->GetBlobToCenterOffset())
{
    SetSliderThicknessPixels(mSliderThicknessPixels);
}

UiSliderSceneProxy::~UiSliderSceneProxy()
{
}

void UiSliderSceneProxy::OnSceneProxyRegistered()
{
    const auto& folderManager = FolderManager::GetInstance();
    ShaderParams shaderParams(
        "UiRectangle Shader",
        folderManager->GetShadersPath() + "uiVS.glsl",
        folderManager->GetShadersPath() + "uiSliderFS.glsl",
        "",
        "",
        "",
        "");
    mUiSliderShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiSliderShader>(shaderParams);
}

void UiSliderSceneProxy::Render()
{
    mUiSliderShader->ExecuteShader();
    mUiSliderShader->LoadRenderSliderLineSubroutine();

    // Render slider background
    glm::vec2 scaleOffset = glm::vec2((mNormalizedScale - (mNormalizedScale * mScale)) * 0.5f);
    mUiSliderShader->SetTransform(mNormalizedTranslation + scaleOffset + mCenterOffset, mNormalizedScale * glm::vec2(mScale));
    mUiSliderShader->SetColor(glm::vec3(0.0f, 0.0f, 0.0f));
    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);

    // Render slider line
    scaleOffset = glm::vec2((mSliderThicknessScale - (mSliderThicknessScale * mScale)) * 0.5f);
    mUiSliderShader->SetTransform(
        mNormalizedTranslation + scaleOffset + mCenterOffset + mSliderToCenterOffset, mSliderThicknessScale * glm::vec2(mScale));
    mUiSliderShader->SetOpacity(mOpacity * mOverlayOpacity);
    mUiSliderShader->SetBorderRadius(10.0f);
    mUiSliderShader->SetWidthHeightPixels(glm::vec2(static_cast<float>(mWidthHightPixels.x), static_cast<float>(20.0f)));
    mUiSliderShader->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);

    // Render slider blob
    mUiSliderShader->LoadRenderSliderBlobSubroutine();
    scaleOffset = glm::vec2((mBlobThicknessScale - (mBlobThicknessScale * mScale)) * 0.5f);
    mUiSliderShader->SetTransform(
        mNormalizedTranslation + scaleOffset + mCenterOffset + mBlobToCenterOffset, mBlobThicknessScale * glm::vec2(mScale));
    mUiSliderShader->SetTransform(
        mNormalizedTranslation + mCenterOffset + mBlobToCenterOffset, mBlobThicknessScale * glm::vec2(mScale));
    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
    mUiSliderShader->StopShader();
}

void UiSliderSceneProxy::SetMaxSliderValue(const float maxSliderValue)
{
    mMaxSliderValue = maxSliderValue;
}

void UiSliderSceneProxy::SetMinSliderValue(const float minSliderValue)
{
    mMinSliderValue = minSliderValue;
}

void UiSliderSceneProxy::SetSliderValue(const float sliderValue)
{
    mSliderValue = sliderValue;
}

void UiSliderSceneProxy::SetSliderStep(const float sliderStep)
{
    mSliderStep = sliderStep;
}

void UiSliderSceneProxy::SetOpacity(const float opacity)
{
    mOpacity = opacity;
}

void UiSliderSceneProxy::SetSliderThicknessPixels(const int32_t thicknessPixels)
{
    mSliderThicknessPixels = thicknessPixels;
}

void UiSliderSceneProxy::SetBlobThicknessPixels(const int32_t thicknessPixels)
{
    mBlobThicknessPixels = thicknessPixels;
}

void UiSliderSceneProxy::SetSliderType(const UiSlider::eUiSliderType sliderType)
{
    mSliderType = sliderType;
}

void UiSliderSceneProxy::SetSliderToCenterOffset(const glm::vec2& offset)
{
    mSliderToCenterOffset = offset;
}

void UiSliderSceneProxy::SetBlobToCenterOffset(const glm::vec2& offset)
{
    mBlobToCenterOffset = offset;
}

void UiSliderSceneProxy::SetSliderThicknessScale(const glm::vec2& scale)
{
    mSliderThicknessScale = scale;
}

void UiSliderSceneProxy::SetBlobThicknessScale(const glm::vec2& scale)
{
    mBlobThicknessScale = scale;
}

void UiSliderSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mUiSliderShader);
}
} // namespace Graphics::Proxy