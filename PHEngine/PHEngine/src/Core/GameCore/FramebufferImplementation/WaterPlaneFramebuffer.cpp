#include "WaterPlaneFramebuffer.h"
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"

using namespace Resources;

namespace EngineCore
{
   namespace FramebufferImpl
   {

      WaterPlaneFramebuffer::WaterPlaneFramebuffer()
      {
         Init();
      }

      WaterPlaneFramebuffer::~WaterPlaneFramebuffer()
      {

      }

      void WaterPlaneFramebuffer::SetTextures()
      {
         TexParams reflectionTexParams(500, 500, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_REPEAT);
         TexParams refractionTexParams(reflectionTexParams);
         TexParams depthTexParams(500, 500, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, 0, GL_DEPTH24_STENCIL8, GL_DEPTH_COMPONENT, GL_FLOAT, GL_REPEAT);

         mReflectionTexture = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(reflectionTexParams);
         mRefractionTexture = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(refractionTexParams);
         mDepthTexture = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(depthTexParams);
         mReflectionFBO.AddRenderTexture(GL_COLOR_ATTACHMENT0, mReflectionTexture);
         mRefractionFBO.AddRenderTexture(GL_COLOR_ATTACHMENT0, mRefractionTexture);
         mRefractionFBO.AddRenderTexture(GL_DEPTH_ATTACHMENT, mDepthTexture);
      }

      void WaterPlaneFramebuffer::SetFramebuffers()
      {
         mReflectionFBO.CreateFramebuffer();
         mRefractionFBO.CreateFramebuffer();
      }

      void WaterPlaneFramebuffer::SetRenderbuffers()
      {
         mReflectionFBO.BindFramebuffer(GL_FRAMEBUFFER, false);
         mReflectionFBO.CreateRenderBuffer(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, mReflectionTexture->GetTextureRezolution());
      }

      void WaterPlaneFramebuffer::CleanUp()
      {
         mReflectionFBO.UnbindFramebuffer();
         mReflectionFBO.CleanUp();
         mRefractionFBO.UnbindFramebuffer();
         mRefractionFBO.CleanUp();
         RenderTargetPool::GetInstance()->TryToFreeMemory(mReflectionTexture);
         RenderTargetPool::GetInstance()->TryToFreeMemory(mRefractionTexture);
         RenderTargetPool::GetInstance()->TryToFreeMemory(mDepthTexture);
      }

   }
}
