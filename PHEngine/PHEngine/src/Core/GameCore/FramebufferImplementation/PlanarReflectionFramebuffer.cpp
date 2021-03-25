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
      }

      PlanarReflectionFramebuffer::~PlanarReflectionFramebuffer()
      {
      }

      void PlanarReflectionFramebuffer::SetTextures()
      {
         TexParams reflectionTexParams(mReflectionViewPortInfo.Width - mReflectionViewPortInfo.OriginX, mReflectionViewPortInfo.Height - mReflectionViewPortInfo.OriginY,
            GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_REPEAT);

         mReflectionTexture = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(reflectionTexParams);
         mReflectionFBO.AddRenderTexture(GL_COLOR_ATTACHMENT0, mReflectionTexture);
      }

      void PlanarReflectionFramebuffer::SetFramebuffers()
      {
         mReflectionFBO.CreateFramebuffer();
      }
      void PlanarReflectionFramebuffer::SetRenderbuffers()
      {
         mReflectionFBO.BindFramebuffer(true);
         mReflectionFBO.CreateRenderBuffer(GL_DEPTH_COMPONENT24, GL_DEPTH_ATTACHMENT, mReflectionTexture->GetTextureRezolution());
      }

      void PlanarReflectionFramebuffer::RenderToTexture()
      {
         RenderToFBO(mReflectionFBO, true, mReflectionViewPortInfo, GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
      }

      void PlanarReflectionFramebuffer::CleanUp()
      {
         mReflectionFBO.UnbindFramebuffer();
         mReflectionFBO.CleanUp();
         RenderTargetPool::GetInstance()->TryToFreeMemory(mReflectionTexture);
      }
   }
}
