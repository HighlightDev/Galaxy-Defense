#include "UiScrollListSceneProxy.h"

#include "Core/GameCore/GUI/UiElements/UiScrollList.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"

#include <gl/glew.h>

using namespace EngineCore::GUI;
using namespace IO;
using namespace Resources;

namespace Graphics {
namespace Proxy {
UiScrollListSceneProxy::UiScrollListSceneProxy(const UiScrollList* uiScrollList)
    : UiSceneProxyBase(uiScrollList)
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

void UiScrollListSceneProxy::Render()
{
    if (mWidthHeightPixels.x == 0 || mWidthHeightPixels.y == 0) {
        return;
    }

    mUiRectangleShader->ExecuteShader();
    mUiRectangleShader->SetTransform(mNormalizedTranslation, mNormalizedScale);
    mUiRectangleShader->SetColor(glm::vec3(0.0f));
    mUiRectangleShader->SetOpacity(0.0f);
    mUiRectangleShader->SetBorderRadius(0.0f);
    mUiRectangleShader->SetWidthHeightPixels(
        glm::vec2(static_cast<float>(mWidthHeightPixels.x), static_cast<float>(mWidthHeightPixels.y)));
    mUiRectangleShader->SetIsRoundTop(false);
    mUiRectangleShader->SetIsRoundBottom(false);

    // Write stencil only — no color output
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    mUiRectangleShader->StopShader();
}

void UiScrollListSceneProxy::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mUiRectangleShader);
}
} // namespace Proxy
} // namespace Graphics
