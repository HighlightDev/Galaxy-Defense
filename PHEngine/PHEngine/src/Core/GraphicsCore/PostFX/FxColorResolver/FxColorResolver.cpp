#include "FxColorResolver.h"

#include "Core/CommonCore/EngineConstants.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderParams.h"
#include "Core/GraphicsCore/Renderer/RenderState.h"
#include "Core/GraphicsCore/Renderer/ResolvedSceneFramebuffer.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"

using namespace Resources;
using namespace EngineUtility;

namespace Graphics {
FxColorResolver::FxColorResolver(const ViewPortInfo& viewPortInfo)
    : mResolveFxColorShader()
    , mViewPortInfo(viewPortInfo)
{
    Init();
}

FxColorResolver::~FxColorResolver()
{
}

void FxColorResolver::Init()
{
    auto shaderParams = ShaderParams("ResolveFxColorShader");
    shaderParams.SetMainShaders(
        IO::FolderManager::GetInstance()->GetAbsolutePath("postFxVS.glsl"),
        IO::FolderManager::GetInstance()->GetAbsolutePath("resolveFxColorFS.glsl"));
    mResolveFxColorShader = ShaderPool::GetInstance()->template GetOrAllocateResource<ResolveFxColorShader>(shaderParams);
}

void FxColorResolver::ExecuteResolveSceneColor(
    const std::shared_ptr<ITexture>& sceneColorTexture, const std::shared_ptr<IPostFxPass>& prevPostFxPass)
{
    RenderState renderState;
    renderState.GetDepthState().SetDepthTestWriteMask(false);
    renderState.BindRenderState();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(0, 0, mViewPortInfo.Width, mViewPortInfo.Height);
    mResolveFxColorShader->ExecuteShader();
    mResolveFxColorShader->SetResolveAlphaFromSource(false);

    if (!prevPostFxPass) {
        ExecutePostFxDisabled(sceneColorTexture);
    } else {
        ExecutePostFxEnabled(sceneColorTexture, prevPostFxPass);
    }

    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLE_STRIP);
    mResolveFxColorShader->StopShader();
    renderState.GetDepthState().SetDepthTestWriteMask(true);
    renderState.BindRenderState();
}

void FxColorResolver::ExecuteResolveGuiColor(
    const std::shared_ptr<ITexture>& sceneColorTexture, const std::shared_ptr<IPostFxPass>& prevPostFxPass)
{
    RenderState renderState;
    renderState.GetDepthState().SetDepthTestWriteMask(false);

    renderState.GetStencilState().SetIsStencilTestEnabled(false);
    // Add compositing of GUI over the already resolved scene by alpha. The content of the UI framebuffer is premultiplied
    // with glBlendFuncSeparate in GuiPass, so we use premultiplied blending: src + dst*(1-srcA).
    renderState.GetBlendingState().SetIsBlendingEnabled(true);
    renderState.GetBlendingState().SetBlendingFunction(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    renderState.BindRenderState();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, mViewPortInfo.Width, mViewPortInfo.Height);

    mResolveFxColorShader->ExecuteShader();
    mResolveFxColorShader->SetResolveAlphaFromSource(true);

    renderState.BindRenderState();

    if (!prevPostFxPass) {
        ExecutePostFxDisabled(sceneColorTexture);
    } else {
        ExecutePostFxEnabled(sceneColorTexture, prevPostFxPass);
    }

    ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLE_STRIP);
    mResolveFxColorShader->StopShader();

    renderState.GetDepthState().SetDepthTestWriteMask(true);
    renderState.GetBlendingState().SetIsBlendingEnabled(false);
    renderState.BindRenderState();
}

void FxColorResolver::ExecutePostFxDisabled(const std::shared_ptr<ITexture>& sceneColorTexture)
{
    sceneColorTexture->BindTexture(0);
    mResolveFxColorShader->SetSceneColorTexture(0);
}

void FxColorResolver::ExecutePostFxEnabled(
    const std::shared_ptr<ITexture>& sceneColorTexture, const std::shared_ptr<IPostFxPass>& prevPostFxPass)
{
    sceneColorTexture->BindTexture(0);
    mResolveFxColorShader->SetSceneColorTexture(0);
    prevPostFxPass->GetPostFxResult()->BindTexture(1);
    mResolveFxColorShader->SetBloomColorTexture(1);
}

void FxColorResolver::CleanUp()
{
    ShaderPool::GetInstance()->TryToFreeMemory(mResolveFxColorShader);
}

void FxColorResolver::ResizeViewPortInfo(const ViewPortInfo& viewPortInfo)
{
    mViewPortInfo = viewPortInfo;
}

} // namespace Graphics
