#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/vec4.hpp>
#include <gl/glew.h>

#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Graphics::Texture;

namespace Graphics
{
   class Framebuffer
   {
      struct RenderAttachmentToTextureBinding
      {
      private:

         using RenderAttachmentToTexture_t = std::unordered_map<int32_t, std::shared_ptr<ITexture>>;

         std::unordered_map<uint32_t, RenderAttachmentToTexture_t> mRenderAttachmentToTextureMap;

      public:
         
         void AddBinding(uint32_t framebufferIndex, int32_t framebufferAttachment, std::shared_ptr<ITexture> texture);

         std::shared_ptr<ITexture> GetTextureByRenderAttachment(uint32_t framebufferIndex, int32_t framebufferAttachement) const;
      };

   protected:

      std::vector<uint32_t> m_framebufferID;
      std::vector<uint32_t> m_renderbufferID;
      RenderAttachmentToTextureBinding mBindings;
      
      virtual void SetTextures() = 0;
      virtual void SetFramebuffers() = 0;
      virtual void SetRenderbuffers() = 0;
      virtual void BindTextureToRenderAttachment() = 0;

      void GenFramebuffers(size_t countFramebuffers);
      void GenRenderbuffers(size_t countRenderbuffers);
      void SetRenderbufferStorage(int32_t renderbufferType, size_t rezolutionWidth, size_t rezolutionHeight) const;
      void SetRenderbufferStorage(int32_t renderbufferType, const glm::ivec2& screenRezolution) const;
      void Attach2DTextureToFramebuffer(int32_t framebufferAttachment, uint32_t attachedTexture) const;
      void Attach2DTextureToFramebuffer(int32_t framebufferAttachment, const std::shared_ptr<ITexture> texture) const;

      void AttachCubeTextureToFramebuffer(int32_t framebufferAttachment, const std::shared_ptr<ITexture> texture) const;

      void AttachRenderbufferToFramebuffer(int32_t framebufferAttachment, size_t renderbufferIndex) const;
      void AttachRenderbufferToFramebuffer(int32_t framebufferAttachment) const;
      void Init();

   private:

      inline GLenum GetFramebufferErrorCode() const;

      void BindFramebufferImpl(size_t framebufferIndex, size_t viewportX, size_t viewportY, size_t viewportWidth, size_t viewportHeight, GLbitfield clearFlag) const;

   public:

      Framebuffer();

      virtual ~Framebuffer();

      void BindFramebuffer(size_t framebufferIndex) const;
      void UnbindFramebuffer() const;
      void BindRenderBuffer(size_t renderbufferIndex) const;

      std::string GetFramebufferLog() const;
      void RenderToFBO(size_t framebufferIndex, size_t viewportX, size_t viewportY, size_t viewportWidth, size_t viewportHeight, GLbitfield clearFlag) const;
      void RenderToFBO(size_t framebufferIndex, const glm::ivec4& viewportRezolution, GLbitfield clearFlag) const;
      virtual void CleanUp();
   };

   inline GLenum Framebuffer::GetFramebufferErrorCode() const
   {
      GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      return result;
   }

}



