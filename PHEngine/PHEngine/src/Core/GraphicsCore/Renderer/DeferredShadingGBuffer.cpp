#include "DeferredShadingGBuffer.h"

namespace Graphics 
{
   // Buffer should be recreated when window size was changed
   DeferredShadingGBuffer::DeferredShadingGBuffer(const ViewPortInfo& viewPortInfo)
      : Framebuffer()
      , mViewPortInfo(viewPortInfo)
   {
      Init();
   }

   DeferredShadingGBuffer::~DeferredShadingGBuffer()
   {
      DestroyGBuffer();
   }

   void DeferredShadingGBuffer::SetTextures()
   {
      // Create all empty textures and attach them to gBuffer Framebuffer Object

      // Depth texture
      {
         TexParams depthParams(mViewPortInfo.Width, mViewPortInfo.Height, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, 0, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, GL_REPEAT, true);
         m_depthBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(depthParams);
      }

      // Position texture
      {
         TexParams positionParams(mViewPortInfo.Width, mViewPortInfo.Height, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, 0, GL_RGB16F, GL_RGB, GL_FLOAT, GL_REPEAT, true);
         m_positionBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(positionParams);
      }

      // Normal texture
      {
         TexParams normalParams(mViewPortInfo.Width, mViewPortInfo.Height, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, 0, GL_RGB16F, GL_RGB, GL_FLOAT, GL_REPEAT, true);
         m_normalBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(normalParams);
      }

      // Albedo + Specular component texture
      {
         TexParams aldbedoSpecParams(mViewPortInfo.Width, mViewPortInfo.Height, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_REPEAT, true);
         m_albedoWithSpecularBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(aldbedoSpecParams);
      }
   }

   void DeferredShadingGBuffer::SetFramebuffers()
   {
      GenFramebuffers(1);
      BindFramebuffer(1);
      Attach2DTextureToFramebuffer(GL_DEPTH_ATTACHMENT, m_depthBuffer);
      Attach2DTextureToFramebuffer(GL_COLOR_ATTACHMENT0, m_positionBuffer);
      Attach2DTextureToFramebuffer(GL_COLOR_ATTACHMENT1, m_normalBuffer);
      Attach2DTextureToFramebuffer(GL_COLOR_ATTACHMENT2, m_albedoWithSpecularBuffer);

      uint32_t attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
      glDrawBuffers(3, attachments);

      UnbindFramebuffer();
   }

   void DeferredShadingGBuffer::SetRenderbuffers()
   {

   }

   void DeferredShadingGBuffer::CleanUp()
   {
      Framebuffer::CleanUp();
      DestroyGBuffer();
   }

   void DeferredShadingGBuffer::DestroyGBuffer()
   {
      RenderTargetPool::GetInstance()->TryToFreeMemory(m_depthBuffer);
      RenderTargetPool::GetInstance()->TryToFreeMemory(m_positionBuffer);
      RenderTargetPool::GetInstance()->TryToFreeMemory(m_normalBuffer);
      RenderTargetPool::GetInstance()->TryToFreeMemory(m_albedoWithSpecularBuffer);
   }

   void DeferredShadingGBuffer::BindDeferredGBuffer()
   {
      BindFramebuffer(1);
      uint32_t attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
      glDrawBuffers(3, attachments);

      glViewport(mViewPortInfo.OriginX, mViewPortInfo.OriginY, mViewPortInfo.Width, mViewPortInfo.Height);
      glClearColor(0, 0, 0, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }

   void DeferredShadingGBuffer::UnbindDeferredGBuffer() {

      UnbindFramebuffer();
      glClearColor(0, 0, 0, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      uint32_t attachment = GL_COLOR_ATTACHMENT0;
      glDrawBuffers(1, &attachment);
   }

   void DeferredShadingGBuffer::BindDepthTexture(int32_t slot) {

      m_depthBuffer->BindTexture(slot);
   }

   void DeferredShadingGBuffer::BindPositionTexture(int32_t slot) {

      m_positionBuffer->BindTexture(slot);
   }

   void DeferredShadingGBuffer::BindNormalTexture(int32_t slot) {

      m_normalBuffer->BindTexture(slot);
   }

   void DeferredShadingGBuffer::BindAlbedoWithSpecularTexture(int32_t slot) {

      m_albedoWithSpecularBuffer->BindTexture(slot);
   }

   void DeferredShadingGBuffer::CopyFramebufferData(size_t srcX, size_t srcY, size_t srcResolutionX, size_t srcResolutionY,
      size_t dstX, size_t dstY, size_t dstResolutionX, size_t dstResolutionY, int32_t bufferBit)
   {
      BindFramebuffer(1);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
      glBlitFramebuffer(srcX, srcY, srcResolutionX, srcResolutionY, dstX, dstY, dstResolutionX, dstResolutionY, bufferBit, GL_NEAREST);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }
}