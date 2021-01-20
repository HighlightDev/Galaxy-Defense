#include "FramebufferBundle.h"
#include "Core/CommonCore/Assertion.h"

namespace Graphics
{

   FramebufferBundle::FramebufferBundle()
   {
   }

   FramebufferBundle::~FramebufferBundle()
   {
   }

   void FramebufferBundle::UnbindFramebuffer() const
   {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glDrawBuffer(GL_COLOR_ATTACHMENT0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }

   void FramebufferBundle::Init()
   {
      SetTextures();
      SetFramebuffers();
      SetRenderbuffers();
   }

   GLenum FramebufferBundle::GetFramebufferErrorCode() const
   {
      GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      return result;
   }

   std::string FramebufferBundle::GetFramebufferLog() const
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

   void FramebufferBundle::SetupViewport(size_t viewportX, size_t viewportY, size_t viewportWidth, size_t viewportHeight, GLbitfield clearFlag) const
   {
      if (0 != clearFlag)
      {
         glClear(clearFlag);
      }
      glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
   }

   void FramebufferBundle::RenderToFBO(const FramebufferObject& fbo, size_t viewportX, size_t viewportY, size_t viewportWidth, size_t viewportHeight, GLbitfield clearFlag) const
   {
      fbo.BindFramebuffer(true);
      SetupViewport(viewportX, viewportY, viewportWidth, viewportHeight, clearFlag);
   }

   void FramebufferBundle::RenderToFBO(const FramebufferObject& fbo, const glm::ivec4& viewport, GLbitfield clearFlag) const
   {
      fbo.BindFramebuffer(true);
      SetupViewport(viewport.x, viewport.y, viewport.z, viewport.w, clearFlag);
   }

   void FramebufferBundle::RenderToFBO(const FramebufferObject& fbo, const ViewPortInfo& viewport, GLbitfield clearFlag) const
   {
      fbo.BindFramebuffer(true);
      SetupViewport(viewport.OriginX, viewport.OriginY, viewport.Width, viewport.Height, clearFlag);
   }
}
