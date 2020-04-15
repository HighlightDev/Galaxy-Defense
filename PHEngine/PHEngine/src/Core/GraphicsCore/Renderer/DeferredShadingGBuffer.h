#pragma once
#include <memory>
#include <iostream>

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"

using namespace Graphics::Texture;
using namespace Resources;

class DeferredShadingGBuffer
{
   using RenderTarget = std::shared_ptr<ITexture>;

   int32_t m_windowWidth;
   int32_t m_windowHeight;

   uint32_t m_gBufferFBO;

   RenderTarget m_depthBuffer;
   RenderTarget m_positionBuffer;
   RenderTarget m_normalBuffer;
   RenderTarget m_albedoWithSpecularBuffer;

public:
   
   // Buffer should be recreated when window size was changed
   DeferredShadingGBuffer(int32_t windowWidth, int32_t windowHeight)
      : m_windowWidth(windowWidth)
      , m_windowHeight(windowHeight)
      , m_gBufferFBO(0)
   {
      InitGBuffer();
   }

   ~DeferredShadingGBuffer() 
   {
      DestroyGBuffer();
   }

   void BindDeferredGBuffer()
   {
      glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBO);
      uint32_t attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
      glDrawBuffers(3, attachments);

      glViewport(0, 0, m_windowWidth, m_windowHeight);
      glClearColor(0, 0, 0, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }

   void UnbindDeferredGBuffer() {

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glClearColor(0, 0, 0, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      uint32_t attachment = GL_COLOR_ATTACHMENT0;
      glDrawBuffers(1, &attachment);
   }

   void BindDepthTexture(int32_t slot) {

      m_depthBuffer->BindTexture(slot);
   }

   void BindPositionTexture(int32_t slot) {

      m_positionBuffer->BindTexture(slot);
   }

   void BindNormalTexture(int32_t slot) {

      m_normalBuffer->BindTexture(slot);
   }

   void BindAlbedoWithSpecularTexture(int32_t slot) {

      m_albedoWithSpecularBuffer->BindTexture(slot);
   }

   inline int32_t GetFramebufferDesc() const
   {
      return m_gBufferFBO;
   }

private:

   void InitGBuffer()
   {
      // Create all empty textures and attach them to gBuffer Framebuffer Object

      glGenFramebuffers(1, &m_gBufferFBO);
      glBindFramebuffer(GL_FRAMEBUFFER, m_gBufferFBO);

      // Depth texture
      {
         TexParams depthParams(m_windowWidth, m_windowHeight, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, 0, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, GL_REPEAT, true);
         m_depthBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(depthParams);
      }
  
      // Position texture
      {
         TexParams positionParams(m_windowWidth, m_windowHeight, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, 0, GL_RGB16F, GL_RGB, GL_FLOAT, GL_REPEAT, true);
         m_positionBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(positionParams);
      }
      
      // Normal texture
      {
         TexParams normalParams(m_windowWidth, m_windowHeight, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, 0, GL_RGB16F, GL_RGB, GL_FLOAT, GL_REPEAT, true);
         m_normalBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(normalParams);
      }

      // Albedo + Specular component texture
      {
         TexParams aldbedoSpecParams(m_windowWidth, m_windowHeight, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_REPEAT, true);
         m_albedoWithSpecularBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(aldbedoSpecParams);
      }

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthBuffer->GetTextureDescriptor(), 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_positionBuffer->GetTextureDescriptor(), 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normalBuffer->GetTextureDescriptor(), 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_albedoWithSpecularBuffer->GetTextureDescriptor(), 0);

      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
         std::cout << "Framebuffer is not complete!" << std::endl;

      uint32_t attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
      glDrawBuffers(3, attachments);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }

   void DestroyGBuffer()
   {
      UnbindDeferredGBuffer();
      glDeleteFramebuffers(1, &m_gBufferFBO);
      RenderTargetPool::GetInstance()->TryToFreeMemory(m_depthBuffer);
      RenderTargetPool::GetInstance()->TryToFreeMemory(m_positionBuffer);
      RenderTargetPool::GetInstance()->TryToFreeMemory(m_normalBuffer);
      RenderTargetPool::GetInstance()->TryToFreeMemory(m_albedoWithSpecularBuffer);
   }

};

