#include "BloomPostFxPass.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderParams.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/GraphicsCore/PostFX/Bloom/BloomConstants.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/GraphicsCore/Renderer/ResolvedSceneFramebuffer.h"
#include "Core/GraphicsCore/Renderer/RenderState.h"

using namespace Resources;
using namespace EngineUtility;

namespace Graphics
{
   BloomPostFxPass::BloomPostFxPass(const ViewPortInfo &viewPortInfo)
       : mBloomFramebuffer(std::make_unique<BloomFramebuffer>(viewPortInfo)),
         mBloomFxShader(),
         mBlurPassCount(1),
         mViewPortInfo(viewPortInfo)
   {
      Init();
   }

   BloomPostFxPass::~BloomPostFxPass()
   {
      if (mBloomFramebuffer)
      {
         mBloomFramebuffer->CleanUp();
      }
   }

   void BloomPostFxPass::Init()
   {
      mBloomFxShader = ShaderPool::GetInstance()->template GetOrAllocateResource<BloomFxShader>(ShaderParams("BloomFxShader",
                                                                                                             IO::FolderManager::GetInstance()->GetShadersPath() + "postFX/postFxVS.glsl",
                                                                                                             IO::FolderManager::GetInstance()->GetShadersPath() + "postFX/bloomFS.glsl",
                                                                                                             "", "", "", ""));
      const auto &cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
      assert(BloomQualitySettings::s_blurQualityMap.count(cfg.BloomQualityName));
      const auto &bloomQuality = BloomQualitySettings::s_blurQualityMap.at(cfg.BloomQualityName);
      mBlurPassCount = bloomQuality.blurPassCount;
   }

   void BloomPostFxPass::ExecutePostFx(const std::shared_ptr<ITexture> &sceneColorTexture, const std::shared_ptr<FramebufferBundle> &previousStepFramebuffer)
   {
      glDepthMask(false);

      const auto &fullscreenViewPort = mBloomFramebuffer->GetFullScreenResolutionViewPortInfo();
      const auto &shrinkedViewPort = mBloomFramebuffer->GetShrinkedResolutionViewPortInfo();

      mBloomFramebuffer->CleanColor1Framebuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      previousStepFramebuffer->CopyFramebufferDataToDstFramebuffer(mBloomFramebuffer->GetColor1FramebufferObjectInstance(),
                                                                   fullscreenViewPort.OriginX, fullscreenViewPort.OriginY, fullscreenViewPort.Width, fullscreenViewPort.Height,
                                                                   shrinkedViewPort.OriginX, shrinkedViewPort.OriginY, shrinkedViewPort.Width, shrinkedViewPort.Height,
                                                                   GL_STENCIL_BUFFER_BIT);

      RenderState<DepthState<false, GL_LEQUAL>, StencilState<true, GL_KEEP, GL_KEEP, GL_REPLACE, GL_NOTEQUAL, 1, 0xFF, 0x00>, BlendingState<false>> renderState;
      renderState.BindRenderState();

      mBloomFxShader->ExecuteShader();
      // extract bright parts for further bluring
      {
         mBloomFramebuffer->BindColor1Framebuffer(0);
         sceneColorTexture->BindTexture(0);
         mBloomFxShader->SetSceneColorTexture(0);
         mBloomFxShader->LoadExtractBrightPartsSubroutine();
         ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
      }

      glDisable(GL_STENCIL_TEST);

      // blur
      {
         for (uint32_t i = 0; i < mBlurPassCount; ++i)
         {
            mBloomFramebuffer->BindColor2Framebuffer(); // vertical blur target
            mBloomFramebuffer->BindColor1Texture(0);
            mBloomFxShader->SetSceneColorTexture(0);
            mBloomFxShader->LoadRunVerticalBlurSubroutine();
            ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);

            mBloomFramebuffer->BindColor1Framebuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // horizontal blur target
            mBloomFramebuffer->BindColor2Texture(0);
            mBloomFxShader->SetSceneColorTexture(0);
            mBloomFxShader->LoadRunHorizontalBlurSubroutine();
            ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
         }
      }

      mBloomFxShader->StopShader();
      glDepthMask(true);
   }

   std::shared_ptr<ITexture> BloomPostFxPass::GetPostFxResult() const
   {
      return mBloomFramebuffer->GetColor1Texture();
   }

   void BloomPostFxPass::CleanUp()
   {
      mBloomFramebuffer->CleanUp();
      mBloomFramebuffer.reset();
      ShaderPool::GetInstance()->TryToFreeMemory(mBloomFxShader);
   }
}
