#include "UiConnectionLineSceneProxy.h"

#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Implementation/GUI/UiConnectionLine.h"

#include <gl/glew.h>

using namespace Resources;
using namespace IO;
using namespace Graphics;
using namespace EngineCore::GUI;

namespace Game {

UiConnectionLineSceneProxy::UiConnectionLineSceneProxy(const UiConnectionLine* uiConnectionLine)
    : UiSceneProxyBase(uiConnectionLine)
    , mStartPoint(uiConnectionLine->GetStartPoint())
    , mEndPoint(uiConnectionLine->GetEndPoint())
    , mColor(uiConnectionLine->GetColor())
    , mThicknessPx(uiConnectionLine->GetThicknessPx())
    , mDashLengthPx(uiConnectionLine->GetDashLengthPx())
    , mGapLengthPx(uiConnectionLine->GetGapLengthPx())
    , mOpacity(uiConnectionLine->GetOpacity())
{
}

UiConnectionLineSceneProxy::~UiConnectionLineSceneProxy()
{
}

void UiConnectionLineSceneProxy::OnSceneProxyRegistered()
{
    const auto& folderManager = FolderManager::GetInstance();
    ShaderParams shaderParams("UiConnectionLine Shader");
    shaderParams.SetMainShaders(
        folderManager->GetAbsolutePath("uiVS.glsl"), folderManager->GetAbsolutePath("uiConnectionLineFS.glsl"));
    mShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiConnectionLineShader>(shaderParams);
}

void UiConnectionLineSceneProxy::Render(
    const std::shared_ptr<Graphics::IPostFxRenderTargetProvider>& postFxRenderTargetProvider,
    const std::shared_ptr<Graphics::CameraSceneProxy>& cameraSceneProxy)
{
    if (mWidthHeightPixels.x == 0 || mWidthHeightPixels.y == 0) {
        return;
    }

    mShader->ExecuteShader();
    const glm::vec2 scaleOffset = glm::vec2((mNormalizedScale - (mNormalizedScale * mScale)) * 0.5f);
    mShader->SetTransform(mNormalizedTranslation + mCenterOffset + scaleOffset, mNormalizedScale * mScale);
    mShader->SetWidthHeightPixels(glm::vec2(static_cast<float>(mWidthHeightPixels.x), static_cast<float>(mWidthHeightPixels.y)));
    mShader->SetStartPoint(mStartPoint);
    mShader->SetEndPoint(mEndPoint);
    mShader->SetColor(mColor);
    mShader->SetThicknessPx(mThicknessPx);
    mShader->SetDashLengthPx(mDashLengthPx);
    mShader->SetGapLengthPx(mGapLengthPx);
    mShader->SetOpacity(mOpacity * mOverlayOpacity);
    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
    mShader->StopShader();
}

void UiConnectionLineSceneProxy::SetStartPoint(const glm::vec2& startPoint)
{
    mStartPoint = startPoint;
}

void UiConnectionLineSceneProxy::SetEndPoint(const glm::vec2& endPoint)
{
    mEndPoint = endPoint;
}

void UiConnectionLineSceneProxy::SetColor(const glm::vec3& color)
{
    mColor = color;
}

void UiConnectionLineSceneProxy::SetThicknessPx(const float thicknessPx)
{
    mThicknessPx = thicknessPx;
}

void UiConnectionLineSceneProxy::SetDashLengthPx(const float dashLengthPx)
{
    mDashLengthPx = dashLengthPx;
}

void UiConnectionLineSceneProxy::SetGapLengthPx(const float gapLengthPx)
{
    mGapLengthPx = gapLengthPx;
}

void UiConnectionLineSceneProxy::SetOpacity(const float opacity)
{
    mOpacity = opacity;
}

void UiConnectionLineSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mShader);
}

} // namespace Game
