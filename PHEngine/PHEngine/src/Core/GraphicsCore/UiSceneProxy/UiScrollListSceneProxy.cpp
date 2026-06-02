#include "UiScrollListSceneProxy.h"

#include "Core/GameCore/GUI/UiElements/UiScrollList.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"

#include <gl/glew.h>

#include <algorithm>

#undef max
#undef min

using namespace EngineCore::GUI;
using namespace IO;
using namespace Resources;

namespace Graphics {
namespace Proxy {

namespace {
constexpr uint8_t c_scrollbarSideNone = static_cast<uint8_t>(eScrollbarSide::NONE);
constexpr uint8_t c_scrollbarSideLeft = static_cast<uint8_t>(eScrollbarSide::LEFT);
constexpr uint8_t c_scrollbarSideRight = static_cast<uint8_t>(eScrollbarSide::RIGHT);

constexpr int32_t c_minThumbHeightPixels = 12;
} // namespace

UiScrollListSceneProxy::UiScrollListSceneProxy(const UiScrollList* uiScrollList)
    : UiSceneProxyBase(uiScrollList)
    , mScrollOffset(uiScrollList->GetScrollOffset())
    , mMaxScrollOffset(uiScrollList->GetMaxScrollOffset())
    , mScrollbarSide(static_cast<uint8_t>(uiScrollList->GetScrollbarSide()))
    , mScrollbarBackgroundColor(uiScrollList->GetScrollbarBackgroundColor())
    , mScrollbarThumbColor(uiScrollList->GetScrollbarThumbColor())
    , mScrollbarThicknessPixels(uiScrollList->GetScrollbarThicknessPixels())
{
}

void UiScrollListSceneProxy::OnSceneProxyRegistered()
{
    const auto& folderManager = FolderManager::GetInstance();
    ShaderParams shaderParams("UiScrollList Stencil Shader");
    shaderParams.SetMainShaders(
        folderManager->GetAbsolutePath("uiVS.glsl"), folderManager->GetAbsolutePath("uiRectangleFS.glsl"));
    mUiRectangleShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiRectangleShader>(shaderParams);
}

void UiScrollListSceneProxy::Render(
    const std::shared_ptr<Graphics::IPostFxRenderTargetProvider>& postFxRenderTargetProvider,
    const std::shared_ptr<Graphics::CameraSceneProxy>& cameraSceneProxy)
{
    if (mWidthHeightPixels.x == 0 || mWidthHeightPixels.y == 0) {
        return;
    }

    mUiRectangleShader->ExecuteShader();

    // Stencil pass for the viewport mask (no color output).
    mUiRectangleShader->SetTransform(mNormalizedTranslation, mNormalizedScale);
    mUiRectangleShader->SetColor(glm::vec3(0.0f));
    mUiRectangleShader->SetOpacity(0.0f);
    mUiRectangleShader->SetBorderRadius(0.0f);
    mUiRectangleShader->SetWidthHeightPixels(
        glm::vec2(static_cast<float>(mWidthHeightPixels.x), static_cast<float>(mWidthHeightPixels.y)));
    mUiRectangleShader->SetIsRoundTop(false);
    mUiRectangleShader->SetIsRoundBottom(false);

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLE_STRIP);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Scrollbar pass (background + thumb).
    if (mScrollbarSide != c_scrollbarSideNone && mScrollbarThicknessPixels > 0 && mMaxScrollOffset > 0) {
        const float viewportWidthPx = static_cast<float>(mWidthHeightPixels.x);
        const float viewportHeightPx = static_cast<float>(mWidthHeightPixels.y);
        // mNormalizedScale is the widget size in [0, 1] root-canvas tex space; this back-derives the root size.
        const float rootWidthPx = viewportWidthPx / std::max(mNormalizedScale.x, 1e-6f);
        const float rootHeightPx = viewportHeightPx / std::max(mNormalizedScale.y, 1e-6f);

        const float thicknessPx = static_cast<float>(mScrollbarThicknessPixels);
        const float thicknessNorm = thicknessPx / rootWidthPx;
        const float bgHeightNorm = mNormalizedScale.y;
        const float bgWidthNorm = thicknessNorm;

        // Background X position: pin to left or right edge of the widget.
        const float bgX = mScrollbarSide == c_scrollbarSideLeft ? mNormalizedTranslation.x
                                                                : mNormalizedTranslation.x + mNormalizedScale.x - bgWidthNorm;
        const float bgY = mNormalizedTranslation.y;

        // Background.
        mUiRectangleShader->SetTransform(glm::vec2(bgX, bgY), glm::vec2(bgWidthNorm, bgHeightNorm));
        mUiRectangleShader->SetColor(mScrollbarBackgroundColor);
        mUiRectangleShader->SetOpacity(mOverlayOpacity);
        mUiRectangleShader->SetBorderRadius(0.0f);
        mUiRectangleShader->SetWidthHeightPixels(glm::vec2(thicknessPx, viewportHeightPx));
        mUiRectangleShader->SetIsRoundTop(false);
        mUiRectangleShader->SetIsRoundBottom(false);
        ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLE_STRIP);

        // Thumb sizing: proportional to viewport / (viewport + maxScroll), clamped to a minimum.
        const float contentHeightPx = viewportHeightPx + static_cast<float>(mMaxScrollOffset);
        float thumbHeightPx = viewportHeightPx * (viewportHeightPx / contentHeightPx);
        thumbHeightPx = std::max(thumbHeightPx, static_cast<float>(c_minThumbHeightPixels));
        thumbHeightPx = std::min(thumbHeightPx, viewportHeightPx);

        // Thumb position: scrollOffset==0 means thumb at top; ==max means thumb at bottom.
        const float travelPx = viewportHeightPx - thumbHeightPx;
        const float scrollT
            = mMaxScrollOffset > 0 ? static_cast<float>(mScrollOffset) / static_cast<float>(mMaxScrollOffset) : 0.0f;
        // viewport Y is bottom-up in tex space; thumb top sits at viewport top minus offset travel.
        const float thumbBottomPx = travelPx * (1.0f - scrollT);

        const float thumbHeightNorm = thumbHeightPx / rootHeightPx;
        const float thumbYNorm = bgY + thumbBottomPx / rootHeightPx;

        mUiRectangleShader->SetTransform(glm::vec2(bgX, thumbYNorm), glm::vec2(bgWidthNorm, thumbHeightNorm));
        mUiRectangleShader->SetColor(mScrollbarThumbColor);
        mUiRectangleShader->SetOpacity(mOverlayOpacity);
        mUiRectangleShader->SetBorderRadius(thicknessPx * 0.5f);
        mUiRectangleShader->SetWidthHeightPixels(glm::vec2(thicknessPx, thumbHeightPx));
        mUiRectangleShader->SetIsRoundTop(true);
        mUiRectangleShader->SetIsRoundBottom(true);
        ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLE_STRIP);
    }

    mUiRectangleShader->StopShader();
}

void UiScrollListSceneProxy::SetScrollOffset(const int32_t value)
{
    mScrollOffset = value;
}

void UiScrollListSceneProxy::SetMaxScrollOffset(const int32_t value)
{
    mMaxScrollOffset = value;
}

void UiScrollListSceneProxy::SetScrollbarSide(const uint8_t value)
{
    mScrollbarSide = value;
}

void UiScrollListSceneProxy::SetScrollbarBackgroundColor(const glm::vec3& value)
{
    mScrollbarBackgroundColor = value;
}

void UiScrollListSceneProxy::SetScrollbarThumbColor(const glm::vec3& value)
{
    mScrollbarThumbColor = value;
}

void UiScrollListSceneProxy::SetScrollbarThicknessPixels(const uint32_t value)
{
    mScrollbarThicknessPixels = value;
}

void UiScrollListSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mUiRectangleShader);
}
} // namespace Proxy
} // namespace Graphics
