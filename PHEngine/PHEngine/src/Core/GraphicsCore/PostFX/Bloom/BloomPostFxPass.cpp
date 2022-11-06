#include "BloomPostFxPass.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderParams.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/GraphicsCore/PostFX/Bloom/BloomConstants.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

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
      mBlurPassCount = BloomQualitySettings::s_blurQualityMap.at(cfg.BloomQualityName).blurPassCount;
   }

   std::shared_ptr<ITexture> BloomPostFxPass::ExecutePostFx(const std::shared_ptr<ITexture> &sceneColorTexture)
   {
      glDepthMask(false);
      mBloomFxShader->ExecuteShader();

      // extract bright parts for further bluring
      {
         mBloomFramebuffer->BindColor1Framebuffer();
         sceneColorTexture->BindTexture(0);
         mBloomFxShader->SetSceneColorTexture(0);
         mBloomFxShader->LoadExtractBrightPartsSubroutine();
         ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
      }

      // blur
      {
         for (int32_t i = 0; i < mBlurPassCount; ++i)
         {
            mBloomFramebuffer->BindColor2Framebuffer(); // vertical blur target
            mBloomFramebuffer->BindColor1Texture(0);
            mBloomFxShader->SetSceneColorTexture(0);
            mBloomFxShader->LoadRunVerticalBlurSubroutine();
            ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);

            mBloomFramebuffer->BindColor1Framebuffer(); // horizontal blur target
            mBloomFramebuffer->BindColor2Texture(0);
            mBloomFxShader->SetSceneColorTexture(0);
            mBloomFxShader->LoadRunHorizontalBlurSubroutine();
            ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
         }
      }

      // mBloomFxShader->StopShader();

      // todo: TEMPORARY!

      mBloomFramebuffer->UnbindFramebuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glViewport(0, 0, mViewPortInfo.Width, mViewPortInfo.Height);
      mBloomFxShader->LoadResolveBloomColorSubroutine();
      sceneColorTexture->BindTexture(0);
      mBloomFxShader->SetSceneColorTexture(0);
      mBloomFramebuffer->BindColor1Texture(1);
      mBloomFxShader->SetBluredColorTexture(1);
      ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
      glDepthMask(true);

      return mBloomFramebuffer->GetColor1Texture();
   }

   void BloomPostFxPass::CleanUp()
   {
      mBloomFramebuffer->CleanUp();
      mBloomFramebuffer.reset();
   }

}
