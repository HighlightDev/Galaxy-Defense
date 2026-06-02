#include "GaussianBlurPostFxPass.h"

#include "Core/GameCore/FramebufferImplementation/BlurFramebuffer.h"
#include "Core/GameCore/ShaderImplementation/BloomFxShader.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferBundle.h"
#include "Core/GraphicsCore/Renderer/RenderState.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

using namespace EngineCore::ShaderImpl;
using namespace EngineCore::FramebufferImpl;
using namespace Graphics::Texture;
using namespace Resources;
using namespace EngineUtility;

namespace Graphics {

GaussianBlurPostFxPass::GaussianBlurPostFxPass(const ViewPortInfo& viewPortInfo)
    : IPostFxPass()
    , mBlurFramebuffer(std::make_unique<BlurFramebuffer>(viewPortInfo))
{
    Init();
}

void GaussianBlurPostFxPass::Init()
{
    auto bloomFxShaderParams = ShaderParams("BloomFxShader");
    bloomFxShaderParams.SetMainShaders(
        IO::FolderManager::GetInstance()->GetAbsolutePath("postFxVS.glsl"),
        IO::FolderManager::GetInstance()->GetAbsolutePath("bloomFS.glsl"));
    mBlurShader = ShaderPool::GetInstance()->template GetOrAllocateResource<BloomFxShader>(bloomFxShaderParams);
    const auto& cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
    mBlurPassCount = 5;
    ext_assert(mBlurPassCount > 0, "BloomPostFxPass::Init: Bloom blur pass count should be greater than 0");
}

GaussianBlurPostFxPass::~GaussianBlurPostFxPass()
{
    if (mBlurFramebuffer) {
        mBlurFramebuffer->CleanUp();
    }
}

void GaussianBlurPostFxPass::ResizeRenderTargets(const ViewPortInfo& viewPortInfo)
{
    if (mBlurFramebuffer) {
        mBlurFramebuffer->ResizeRenderTargets(viewPortInfo);
    }
}

void GaussianBlurPostFxPass::ExecutePostFx(
    const std::shared_ptr<ITexture>& sceneColorTexture,
    [[maybe_unused]] const std::shared_ptr<FramebufferBundle>& previousStepFramebuffer)
{

    RenderState renderState;
    renderState.GetDepthState().SetIsDepthTestEnabled(false);
    renderState.GetStencilState().SetIsStencilTestEnabled(false);
    renderState.GetBlendingState().SetIsBlendingEnabled(false);
    renderState.BindRenderState();

    mBlurShader->ExecuteShader();

    for (int32_t i = 0; i < mBlurPassCount; ++i) {

        mBlurFramebuffer->BindColor1Framebuffer(GL_COLOR_BUFFER_BIT);
        constexpr int32_t sceneColorTextureSlot = 0;
        sceneColorTexture->BindTexture(sceneColorTextureSlot);
        mBlurShader->SetSceneColorTexture(sceneColorTextureSlot);
        mBlurShader->LoadRunVerticalBlurSubroutine();
        ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLE_STRIP);

        constexpr int32_t verticalPassTextureSlot = 0;
        mBlurFramebuffer->BindColor2Framebuffer(GL_COLOR_BUFFER_BIT);
        mBlurFramebuffer->BindColor1Texture(verticalPassTextureSlot);
        mBlurShader->SetSceneColorTexture(verticalPassTextureSlot);
        mBlurShader->LoadRunHorizontalBlurSubroutine();
        ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLE_STRIP);
    }
}

std::shared_ptr<ITexture> GaussianBlurPostFxPass::GetPostFxResult() const
{
    return mBlurFramebuffer->GetColor2Texture();
}

void GaussianBlurPostFxPass::CleanUp()
{
    if (mBlurFramebuffer) {
        mBlurFramebuffer->CleanUp();
        mBlurFramebuffer.reset();
    }
    if (mBlurShader) {
        ShaderPool::GetInstance()->TryToFreeMemory(mBlurShader);
    }
}

} // namespace Graphics