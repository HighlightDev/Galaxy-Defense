#include "PlanarReflectionFramebuffer.h"
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"

using namespace Resources;

namespace Game {

   namespace FramebufferImpl {

      PlanarReflectionFramebuffer::PlanarReflectionFramebuffer(const ViewPortInfo& reflectionViewPortInfo)
         : FramebufferBundle()
         , mReflectionViewPortInfo(reflectionViewPortInfo)
      {
         Init();
         mReflectionRenderTargetSurface.CheckErrors();
         mReflectionTextureSurface.CheckErrors();
         glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }

      PlanarReflectionFramebuffer::~PlanarReflectionFramebuffer()
      {
      }

      void PlanarReflectionFramebuffer::SetTextures()
      {
         TexParams reflectionTexParams(mReflectionViewPortInfo.Width - mReflectionViewPortInfo.OriginX, mReflectionViewPortInfo.Height - mReflectionViewPortInfo.OriginY,
            GL_TEXTURE_2D, GL_NEAREST, GL_LINEAR, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_REPEAT, true);

         TexParams reflectionSurfaceParams(mReflectionViewPortInfo.Width - mReflectionViewPortInfo.OriginX, mReflectionViewPortInfo.Height - mReflectionViewPortInfo.OriginY,
            GL_TEXTURE_2D, GL_NEAREST, GL_LINEAR, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_REPEAT, true);

         mReflectionTexture = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(reflectionTexParams);
         mReflectionTextureSurface.AddRenderTexture(GL_COLOR_ATTACHMENT0, mReflectionTexture);

         mReflectionSurfaceTarget = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(reflectionSurfaceParams);
         mReflectionRenderTargetSurface.AddRenderTexture(GL_COLOR_ATTACHMENT0, mReflectionSurfaceTarget);
      }

      void PlanarReflectionFramebuffer::SetFramebuffers()
      {
         mReflectionRenderTargetSurface.CreateFramebuffer();
         mReflectionTextureSurface.CreateFramebuffer();
      }
      void PlanarReflectionFramebuffer::SetRenderbuffers()
      {
         mReflectionRenderTargetSurface.BindFramebuffer(GL_FRAMEBUFFER, true);
         mReflectionRenderTargetSurface.CreateRenderBuffer(GL_DEPTH_COMPONENT24, GL_DEPTH_ATTACHMENT, mReflectionTexture->GetTextureRezolution());
      }

      void PlanarReflectionFramebuffer::RenderToTexture()
      {
         RenderToFBO(mReflectionRenderTargetSurface, true, mReflectionViewPortInfo, GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
      }

      void PlanarReflectionFramebuffer::CleanUp()
      {
         mReflectionRenderTargetSurface.UnbindFramebuffer();
         mReflectionRenderTargetSurface.CleanUp();
         RenderTargetPool::GetInstance()->TryToFreeMemory(mReflectionSurfaceTarget);

         mReflectionTextureSurface.UnbindFramebuffer();
         mReflectionTextureSurface.CleanUp();
         RenderTargetPool::GetInstance()->TryToFreeMemory(mReflectionTexture);
      }

      void PlanarReflectionFramebuffer::ResolveReflectionRenderTargetSurfaceData()
      {
         auto rezolution = mReflectionTexture->GetTextureRezolution();
         mReflectionRenderTargetSurface.BindFramebuffer(GL_READ_FRAMEBUFFER, true, false);
         mReflectionTextureSurface.BindFramebuffer(GL_DRAW_FRAMEBUFFER, true, false);

         glBlitFramebuffer(0, 0, rezolution.x, rezolution.y, 0, 0, rezolution.x, rezolution.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);

         mReflectionTextureSurface.UnbindFramebuffer();
      }

      std::shared_ptr<ITexture> PlanarReflectionFramebuffer::GetReflectionTexture() const 
      {
         return mReflectionTexture;
      }
   }
}
