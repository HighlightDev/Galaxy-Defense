#include "WaterPlaneFramebuffer.h"
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"

using namespace Resources;

namespace Game
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
         ReflectionTexture = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(reflectionTexParams);
         RefractionTexture = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(refractionTexParams);
         DepthTexture = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(depthTexParams);
      }

      void WaterPlaneFramebuffer::SetFramebuffers()
      {
         /*Create 2 framebuffers :
            1 - for reflection
            2 - for refraction*/
         GenFramebuffers(2);
         BindFramebuffer(1);
         Attach2DTextureToFramebuffer(GL_COLOR_ATTACHMENT0, ReflectionTexture->GetTextureDescriptor());
         BindFramebuffer(2);
         Attach2DTextureToFramebuffer(GL_COLOR_ATTACHMENT0, RefractionTexture->GetTextureDescriptor());
         Attach2DTextureToFramebuffer(GL_DEPTH_ATTACHMENT, DepthTexture->GetTextureDescriptor());
      }

      void WaterPlaneFramebuffer::SetRenderbuffers()
      {
         /*Attach 1 depthbuffer*/
         GenRenderbuffers(1);
         BindFramebuffer(1);
         BindRenderBuffer(1);
         SetRenderbufferStorage(GL_DEPTH24_STENCIL8, ReflectionTexture->GetTextureRezolution());
         AttachRenderbufferToFramebuffer(GL_DEPTH_STENCIL_ATTACHMENT);
      }

      void WaterPlaneFramebuffer::CleanUp()
      {
         RenderTargetPool::GetInstance()->TryToFreeMemory(ReflectionTexture);
         RenderTargetPool::GetInstance()->TryToFreeMemory(RefractionTexture);
         RenderTargetPool::GetInstance()->TryToFreeMemory(DepthTexture);
         Framebuffer::CleanUp();          
      }

   }
}
