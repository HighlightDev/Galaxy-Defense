#include "BloomPostFxPass.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderParams.h"
#include "Core/IoCore/FolderManager.h"

namespace Graphics
{
   BloomPostFxPass::BloomPostFxPass(const ViewPortInfo &viewPortInfo)
       : mBloomFramebuffer(std::make_unique<BloomFramebuffer>(viewPortInfo)),
         mBloomFxShader(std::make_unique<BloomFxShader>(ShaderParams("BloomFxShader",
                                                                     IO::FolderManager::GetInstance()->GetShadersPath() + "postFX/postFxVS.glsl",
                                                                     IO::FolderManager::GetInstance()->GetShadersPath() + "postFX/bloomFS.glsl",
                                                                     "", "", "", ""))),
         mViewPortInfo(viewPortInfo)
   {
   }

   BloomPostFxPass::~BloomPostFxPass()
   {
      if (mBloomFramebuffer)
      {
         mBloomFramebuffer->CleanUp();
      }
   }

   std::shared_ptr<ITexture> BloomPostFxPass::ExecutePostFx(const std::shared_ptr<ITexture> &sceneColorTexture)
   {
      // extract bright parts for further bluring
      {
         mBloomFramebuffer->BindColor1Framebuffer();
         mBloomFxShader->ExecuteShader();
         sceneColorTexture->BindTexture(0);
         mBloomFxShader->SetSceneColorTexture(0);
         mBloomFxShader->LoadExtractBrightPartsSubroutine();
         ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
         mBloomFxShader->StopShader();
         mBloomFramebuffer->UnbindFramebuffer(0);
      }

      return mBloomFramebuffer->GetColor1Texture();
   }

   void BloomPostFxPass::CleanUp()
   {
      mBloomFramebuffer->CleanUp();
      mBloomFramebuffer.reset();
   }

}
