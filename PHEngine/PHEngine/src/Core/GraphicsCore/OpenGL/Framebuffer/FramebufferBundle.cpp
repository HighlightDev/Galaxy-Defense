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

   void FramebufferBundle::SetupViewport(size_t viewportX, size_t viewportY, size_t viewportWidth, size_t viewportHeight, GLbitfield clearFlag) const
   {
      if (0 != clearFlag)
      {
         glClear(clearFlag);
      }
      glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
   }

   void FramebufferBundle::RenderToFBO(const FramebufferObject& fbo, bool bBindFramebuffer, size_t viewportX, size_t viewportY, size_t viewportWidth, size_t viewportHeight, GLbitfield clearFlag) const
   {
      fbo.BindFramebuffer(GL_FRAMEBUFFER, bBindFramebuffer, true);
      SetupViewport(viewportX, viewportY, viewportWidth, viewportHeight, clearFlag);
   }

   void FramebufferBundle::RenderToFBO(const FramebufferObject& fbo, bool bBindFramebuffer, const glm::ivec4& viewport, GLbitfield clearFlag) const
   {
      fbo.BindFramebuffer(GL_FRAMEBUFFER, bBindFramebuffer, true);
      SetupViewport(viewport.x, viewport.y, viewport.z, viewport.w, clearFlag);
   }

   void FramebufferBundle::RenderToFBO(const FramebufferObject& fbo, bool bBindFramebuffer, const ViewPortInfo& viewport, GLbitfield clearFlag) const
   {
      fbo.BindFramebuffer(GL_FRAMEBUFFER, bBindFramebuffer, true);
      SetupViewport(viewport.OriginX, viewport.OriginY, viewport.Width, viewport.Height, clearFlag);
   }
}
