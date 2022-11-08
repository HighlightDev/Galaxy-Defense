#include "FxColorResolver.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderParams.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

using namespace Resources;
using namespace EngineUtility;

namespace Graphics
{
   FxColorResolver::FxColorResolver(const ViewPortInfo &viewPortInfo)
       : mResolveFxColorShader(),
         mViewPortInfo(viewPortInfo)
   {
      Init();
   }

   FxColorResolver::~FxColorResolver()
   {
   }

   void FxColorResolver::Init()
   {
      mResolveFxColorShader = ShaderPool::GetInstance()->template GetOrAllocateResource<ResolveFxColorShader>(ShaderParams("ResolveFxColorShader",
                                                                                                                           IO::FolderManager::GetInstance()->GetShadersPath() + "postFX/postFxVS.glsl",
                                                                                                                           IO::FolderManager::GetInstance()->GetShadersPath() + "postFX/resolveFxColorFS.glsl",
                                                                                                                           "", "", "", ""));
      const auto &cfg = EngineConfigHolder::GetInstance()->GetEngineConfig();
   }

   void FxColorResolver::Execute(const std::shared_ptr<ITexture> &sceneColorTexture, const std::shared_ptr<IPostFxPass> &prevPostFxPass)
   {
      glDepthMask(false);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glDrawBuffer(GL_COLOR_ATTACHMENT0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glViewport(0, 0, mViewPortInfo.Width, mViewPortInfo.Height);
      mResolveFxColorShader->ExecuteShader();

      if (!prevPostFxPass)
      {
         ExecutePostFxDisabled(sceneColorTexture);
      }
      else 
      {
         ExecutePostFxEnabled(sceneColorTexture, prevPostFxPass);
      }

      ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
      mResolveFxColorShader->StopShader();
      glDepthMask(true);
   }

   void FxColorResolver::ExecutePostFxDisabled(const std::shared_ptr<ITexture> &sceneColorTexture)
   {
      sceneColorTexture->BindTexture(0);
      mResolveFxColorShader->SetSceneColorTexture(0);
   }

   void FxColorResolver::ExecutePostFxEnabled(const std::shared_ptr<ITexture> &sceneColorTexture, const std::shared_ptr<IPostFxPass> &prevPostFxPass)
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

}
