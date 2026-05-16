#include "UiRectangleSceneProxy.h"

#include "Core/GameCore/GUI/UiElements/UiRectangle.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
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

void UiRectangleSceneProxy::Render()
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
    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
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

void UiRectangleSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mUiRectangleShader);
}
} // namespace Proxy
} // namespace Graphics