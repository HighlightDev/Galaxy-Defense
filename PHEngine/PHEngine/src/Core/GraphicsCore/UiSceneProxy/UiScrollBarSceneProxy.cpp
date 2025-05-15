#include "UiScrollBarSceneProxy.h"

#include "Core/GameCore/GUI/UiElements/UiScrollBar.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"

#include <gl/glew.h>

using namespace Resources;
using namespace IO;
using namespace EngineCore::GUI;

namespace Graphics::Proxy {
UiScrollBarSceneProxy::UiScrollBarSceneProxy(const UiScrollBar* uiScrollBar)
    : UiSceneProxyBase(uiScrollBar)
    , mMaxScrollValue(uiScrollBar->GetMaxScrollValue())
    , mMinScrollValue(uiScrollBar->GetMinScrollValue())
    , mScrollValue(uiScrollBar->GetScrollValue())
    , mScrollStep(uiScrollBar->GetScrollStep())
{
}

UiScrollBarSceneProxy::~UiScrollBarSceneProxy()
{
}

void UiScrollBarSceneProxy::OnSceneProxyRegistered()
{
    const auto& folderManager = FolderManager::GetInstance();
    ShaderParams shaderParams(
        "UiRectangle Shader",
        folderManager->GetShadersPath() + "uiVS.glsl",
        folderManager->GetShadersPath() + "uiRectangleFS.glsl",
        "",
        "",
        "",
        "");
    mUiRectangleShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiRectangleShader>(shaderParams);
}

void UiScrollBarSceneProxy::Render()
{
    mUiRectangleShader->ExecuteShader();
    const glm::vec2 scaleOffset = glm::vec2((mNormalizedScale - (mNormalizedScale * mScale)) * 0.5f);
    mUiRectangleShader->SetTransform(mNormalizedTranslation + scaleOffset + mCenterOffset, mNormalizedScale * glm::vec2(mScale));
    mUiRectangleShader->SetWidthHeightPixels(
        glm::vec2(static_cast<float>(mWidthHightPixels.x), static_cast<float>(mWidthHightPixels.y)));
    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
    mUiRectangleShader->StopShader();
}

void UiScrollBarSceneProxy::SetMaxScrollValue(const float maxScrollValue)
{
    mMaxScrollValue = maxScrollValue;
}

void UiScrollBarSceneProxy::SetMinScrollValue(const float minScrollValue)
{
    mMinScrollValue = minScrollValue;
}

void UiScrollBarSceneProxy::SetScrollValue(const float scrollValue)
{
    mScrollValue = scrollValue;
}

void UiScrollBarSceneProxy::SetScrollStep(const float scrollStep)
{
    mScrollStep = scrollStep;
}

void UiScrollBarSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mUiRectangleShader);
}
} // namespace Graphics::Proxy