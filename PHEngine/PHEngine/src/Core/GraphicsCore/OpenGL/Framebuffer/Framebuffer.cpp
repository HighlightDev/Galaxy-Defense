#include "Framebuffer.h"

namespace Graphics
{

   Framebuffer::Framebuffer()
   {
   }

   Framebuffer::~Framebuffer()
   {
   }

   void Framebuffer::GenFramebuffers(size_t countFramebuffers)
   {
      GLuint* framebuffers = new GLuint[countFramebuffers];

      glGenFramebuffers(countFramebuffers, framebuffers);
      for (size_t i = 0; i < countFramebuffers; i++)
         m_framebufferID.push_back(framebuffers[i]);

      delete framebuffers;
   }

   void Framebuffer::GenRenderbuffers(size_t countRenderbuffers)
   {
      GLuint* renderbuffers = new GLuint[countRenderbuffers];

      glGenRenderbuffers(countRenderbuffers, renderbuffers);
      for (size_t i = 0; i < countRenderbuffers; i++)
         m_framebufferID.push_back(renderbuffers[i]);

      delete renderbuffers;
   }

   void Framebuffer::BindFramebuffer(size_t framebufferIndex) const
   {
      glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID[framebufferIndex - 1]);
   }

   void Framebuffer::UnbindFramebuffer() const
   {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }

   void Framebuffer::BindRenderBuffer(size_t renderbufferIndex) const
   {
      glBindRenderbuffer(GL_FRAMEBUFFER, m_renderbufferID[renderbufferIndex - 1]);
   }

   void Framebuffer::SetRenderbufferStorage(int32_t renderbufferType, size_t rezolutionWidth, size_t rezolutionHeight) const
   {
      glRenderbufferStorage(GL_RENDERBUFFER, renderbufferType, rezolutionWidth, rezolutionHeight);
   }

   void Framebuffer::SetRenderbufferStorage(int32_t renderbufferType, const glm::ivec2& screenRezolution) const
   {
      glRenderbufferStorage(GL_RENDERBUFFER, renderbufferType, screenRezolution.x, screenRezolution.y);
   }

   void Framebuffer::Attach2DTextureToFramebuffer(int32_t framebufferAttachment, uint32_t attachedTexture) const
   {
      glFramebufferTexture2D(GL_FRAMEBUFFER, framebufferAttachment, GL_TEXTURE_2D, attachedTexture, 0);
   }

   void Framebuffer::Attach2DTextureToFramebuffer(int32_t framebufferAttachment, const std::shared_ptr<ITexture> texture) const
   {
      glFramebufferTexture2D(GL_FRAMEBUFFER, framebufferAttachment, GL_TEXTURE_2D, texture->GetTextureDescriptor(), 0);
   }

   void Framebuffer::AttachCubeTextureToFramebuffer(int32_t framebufferAttachment, const std::shared_ptr<ITexture> texture) const
   {
      glFramebufferTexture(GL_FRAMEBUFFER, framebufferAttachment, texture->GetTextureDescriptor(), 0);
   }

   void Framebuffer::AttachRenderbufferToFramebuffer(int32_t framebufferAttachment, size_t renderbufferIndex) const
   {
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, framebufferAttachment, GL_RENDERBUFFER,
         m_renderbufferID[renderbufferIndex - 1]);
   }

   void Framebuffer::AttachRenderbufferToFramebuffer(int32_t framebufferAttachment) const
   {
      if (m_renderbufferID.size() > 0)
      {
         glFramebufferRenderbuffer(GL_FRAMEBUFFER, framebufferAttachment, GL_RENDERBUFFER,
            m_renderbufferID[0]);
      }
   }

   void Framebuffer::Init()
   {
      SetTextures();
      SetFramebuffers();
      SetRenderbuffers();
   }

   void Framebuffer::BindFramebufferImpl(size_t framebufferIndex, size_t viewportX, size_t viewportY, size_t viewportWidth, size_t viewportHeight, GLbitfield clearFlag) const
   {
      BindFramebuffer(framebufferIndex);
      if (0 != clearFlag)
      {
         glClear(clearFlag);
      }
      glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
   }

   std::string Framebuffer::GetFramebufferLog() const
   {
      GLenum errorCode = GetFramebufferErrorCode();
      switch (errorCode)
      {
         case GL_FRAMEBUFFER_COMPLETE: { return "Framebuffer : complete."; }
         case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: { return "Framebuffer : Not all framebuffer attachment points are framebuffer attachment complete."; }
         case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: { return "Framebuffer : Not all attached images have the same width and height."; }
         case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: { return "Framebuffer : Each draw buffer must specify color attachment points that have images attached or must be GL_NONE."; }
         case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: { return "Framebuffer : Incomplete formats"; }
         case GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_EXT: { return "Framebuffer : Incomplete layer count"; }
         case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: { return "Framebuffer : All attachments must be layered attachments."; }
         case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: { return "Framebuffer : No images are attached to the framebuffer."; }
         case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: { return "Framebuffer : All images must have the same number of multisample samples."; }
         case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: { return "Framebuffer : ReadBuffer must specify an attachment point that has an image attached. "; }
         case GL_FRAMEBUFFER_UNDEFINED: { return "Framebuffer : FBO object number 0 is bound"; }
         case GL_FRAMEBUFFER_UNSUPPORTED: { return "Framebuffer : The combination of internal formats of the attached images violates an implementation-dependent set of restrictions."; }
         default: return "Framebuffer : Undefined error.";
      }
   }

   void Framebuffer::RenderToFBO(size_t framebufferIndex, size_t viewportX, size_t viewportY, size_t viewportWidth, size_t viewportHeight, GLbitfield clearFlag) const
   {
      BindFramebufferImpl(framebufferIndex, viewportX, viewportY, viewportWidth, viewportHeight, clearFlag);
   }

   void Framebuffer::RenderToFBO(size_t framebufferIndex, const glm::ivec4& viewport, GLbitfield clearFlag) const
   {
      BindFramebufferImpl(framebufferIndex, viewport.x, viewport.y, viewport.z, viewport.w, clearFlag);
   }

   void Framebuffer::CleanUp()
   {
      UnbindFramebuffer();
      glDeleteRenderbuffers(m_renderbufferID.size(), m_renderbufferID.data());
      glDeleteFramebuffers(m_framebufferID.size(), m_framebufferID.data());
   }
}
