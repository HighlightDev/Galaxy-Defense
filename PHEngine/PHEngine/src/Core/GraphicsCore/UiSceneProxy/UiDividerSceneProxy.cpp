#include "UiDividerSceneProxy.h"

#include "Core/GameCore/GUI/UiElements/UiDivider.h"
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
UiDividerSceneProxy::UiDividerSceneProxy(const UiDivider* uiDivider)
    : UiSceneProxyBase(uiDivider)
    , mColor(uiDivider->GetColor())
    , mOpacity(uiDivider->GetOpacity())
    , mLineWidthPx(uiDivider->GetLineWidthPx())
    , mEdgeFade(uiDivider->GetEdgeFade())
    , mOrientation(uiDivider->GetDividerOrientation())
{
}

UiDividerSceneProxy::~UiDividerSceneProxy()
{
}

void UiDividerSceneProxy::OnSceneProxyRegistered()
{
    const auto& folderManager = FolderManager::GetInstance();
    ShaderParams shaderParams("UiDivider Shader");
    shaderParams.SetMainShaders(
        folderManager->GetAbsolutePathToRes("uiVS.glsl"), folderManager->GetAbsolutePathToRes("uiDividerFS.glsl"));
    mUiDividerShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiDividerShader>(shaderParams);
}

void UiDividerSceneProxy::Render(
    const std::shared_ptr<Graphics::IPostFxRenderTargetProvider>& postFxRenderTargetProvider,
    const std::shared_ptr<CameraSceneProxy>& cameraSceneProxy)
{
    if (mWidthHeightPixels.x == 0 || mWidthHeightPixels.y == 0) {
        return;
    }

    mUiDividerShader->ExecuteShader();
    const glm::vec2 scaleOffset = glm::vec2((mNormalizedScale - (mNormalizedScale * mScale)) * 0.5f);
    mUiDividerShader->SetTransform(mNormalizedTranslation + scaleOffset + mCenterOffset, mNormalizedScale * mScale);
    mUiDividerShader->SetColor(mColor);
    mUiDividerShader->SetOpacity(mOpacity * mOverlayOpacity);
    mUiDividerShader->SetLineWidthPx(mLineWidthPx);
    mUiDividerShader->SetEdgeFade(mEdgeFade);
    mUiDividerShader->SetOrientation(static_cast<int32_t>(mOrientation));
    mUiDividerShader->SetWidthHeightPixels(
        glm::vec2(static_cast<float>(mWidthHeightPixels.x), static_cast<float>(mWidthHeightPixels.y)));

    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLE_STRIP);
    mUiDividerShader->StopShader();
}

void UiDividerSceneProxy::SetColor(const glm::vec3& color)
{
    mColor = color;
}

void UiDividerSceneProxy::SetOpacity(const float opacity)
{
    mOpacity = opacity;
}

void UiDividerSceneProxy::SetLineWidthPx(const float widthPx)
{
    mLineWidthPx = widthPx;
}

void UiDividerSceneProxy::SetEdgeFade(const float edgeFade)
{
    mEdgeFade = edgeFade;
}

void UiDividerSceneProxy::SetDividerOrientation(const eDividerOrientation orientation)
{
    mOrientation = orientation;
}

void UiDividerSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mUiDividerShader);
}
} // namespace Proxy
} // namespace Graphics