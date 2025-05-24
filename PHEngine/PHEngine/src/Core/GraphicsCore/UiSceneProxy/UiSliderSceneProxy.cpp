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
    , mSliderType(uiSliderBar->GetSliderType())
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
    const glm::vec2 scaleOffset
        = glm::vec2((mSliderThicknessScale - (mSliderThicknessScale * mScale)) * 0.5f);
    mUiSliderShader->SetTransform(
        mNormalizedTranslation + scaleOffset + mCenterOffset, mSliderThicknessScale * glm::vec2(mScale));
    mUiSliderShader->SetOpacity(mOpacity * mOverlayOpacity);
    mUiSliderShader->SetBorderRadius(10.0f);
    mUiSliderShader->SetWidthHeightPixels(glm::vec2(static_cast<float>(mWidthHightPixels.x), static_cast<float>(20.0f)));
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
    if (const auto& parentCanvasSp = mParentCanvasProxy.lock()) {
        const auto& canvasWidthHightPixels = parentCanvasSp->GetWidthHeight();
        if (canvasWidthHightPixels.x > 0 && canvasWidthHightPixels.y > 0) {

            const float sliderThicknessSide
                = UiSlider::eUiSliderType::Horizontal == mSliderType ? canvasWidthHightPixels.y : canvasWidthHightPixels.x;
            if (mSliderThicknessPixels > sliderThicknessSide) {
                mSliderThicknessPixels = sliderThicknessSide;
            }
            const float mSliderThicknessNormalized
                = static_cast<float>(mSliderThicknessPixels) / static_cast<float>(sliderThicknessSide);
            mSliderThicknessScale = glm::vec2(
                UiSlider::eUiSliderType::Horizontal == mSliderType ? mNormalizedScale.x : mSliderThicknessNormalized,
                UiSlider::eUiSliderType::Vertical == mSliderType ? mNormalizedScale.y : mSliderThicknessNormalized);
        }
    }
}

void UiSliderSceneProxy::SetSliderType(const UiSlider::eUiSliderType sliderType)
{
    mSliderType = sliderType;
    SetSliderThicknessPixels(mSliderThicknessPixels); // Update thickness scale based on new type
}

void UiSliderSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mUiSliderShader);
}
} // namespace Graphics::Proxy