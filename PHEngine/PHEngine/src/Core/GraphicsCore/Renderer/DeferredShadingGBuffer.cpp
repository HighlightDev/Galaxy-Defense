#include "DeferredShadingGBuffer.h"

namespace Graphics 
{
   // Buffer should be recreated when window size was changed
   DeferredShadingGBuffer::DeferredShadingGBuffer(const ViewPortInfo& viewPortInfo)
      : FramebufferBundle()
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
      // Create all empty textures and attach them to gBuffer FramebufferBundle Object

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
   
      mFramebuffer.AddRenderTexture(GL_DEPTH_ATTACHMENT, m_depthBuffer);
      mFramebuffer.AddRenderTexture(GL_COLOR_ATTACHMENT0, m_positionBuffer);
      mFramebuffer.AddRenderTexture(GL_COLOR_ATTACHMENT1, m_normalBuffer);
      mFramebuffer.AddRenderTexture(GL_COLOR_ATTACHMENT2, m_albedoWithSpecularBuffer);
   }

   void DeferredShadingGBuffer::SetFramebuffers()
   {
      mFramebuffer.CreateFramebuffer();
   }

   void DeferredShadingGBuffer::SetRenderbuffers()
   {
   }

   void DeferredShadingGBuffer::CleanUp()
   {
      DestroyGBuffer();
   }

   void DeferredShadingGBuffer::DestroyGBuffer()
   {
      mFramebuffer.UnbindFramebuffer();
      mFramebuffer.CleanUp();
      RenderTargetPool::GetInstance()->TryToFreeMemory(m_depthBuffer);
      RenderTargetPool::GetInstance()->TryToFreeMemory(m_positionBuffer);
      RenderTargetPool::GetInstance()->TryToFreeMemory(m_normalBuffer);
      RenderTargetPool::GetInstance()->TryToFreeMemory(m_albedoWithSpecularBuffer);
   }

   void DeferredShadingGBuffer::BindDeferredGBuffer()
   {
      RenderToFBO(mFramebuffer, true, mViewPortInfo, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }

   void DeferredShadingGBuffer::UnbindDeferredGBuffer() {

      UnbindFramebuffer();
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
      mFramebuffer.BindFramebuffer(true, false);

      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
      glBlitFramebuffer(srcX, srcY, srcResolutionX, srcResolutionY, dstX, dstY, dstResolutionX, dstResolutionY, bufferBit, GL_NEAREST);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }
}