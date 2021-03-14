#include "FramebufferObject.h"
#include "Core/CommonCore/Assertion.h"

#include <algorithm>

namespace Graphics
{

   FramebufferObject::FramebufferObject()
      : mFramebufferId(std::numeric_limits<uint32_t>::max())
      , mRenderBufferId(std::numeric_limits<uint32_t>::max())
   {
   }

   FramebufferObject::~FramebufferObject()
   {
   }

   void FramebufferObject::AddRenderTexture(uint32_t framebufferAttachement, std::shared_ptr<ITexture> renderTexture)
   {
      assert(mRenderTextures.count(framebufferAttachement) == 0);
      mRenderTextures[framebufferAttachement] = renderTexture;
   }

   // should be called after all render textures are attached
   void FramebufferObject::CreateFramebuffer()
   {
      assert(mRenderTextures.size() > 0);
      glGenFramebuffers(1, &mFramebufferId);
      glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferId);

      for (const auto& attachmentToTexture : mRenderTextures)
      {
         const auto& renderTexture = attachmentToTexture.second;
         if (renderTexture->GetTextureType() == TextureType::TEXTURE_2D)
         {
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentToTexture.first, GL_TEXTURE_2D, renderTexture->GetTextureDescriptor(), 0);
         }
         else if (renderTexture->GetTextureType() == TextureType::TEXTURE_CUBE)
         {
            glFramebufferTexture(GL_FRAMEBUFFER, attachmentToTexture.first, renderTexture->GetTextureDescriptor(), 0);
         }
      }

      assert(GetFramebufferErrorCode() == GL_FRAMEBUFFER_COMPLETE);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      CollectAttachments();
   }

   GLenum FramebufferObject::GetFramebufferErrorCode() const
   {
      GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      return result;
   }

   std::string FramebufferObject::GetFramebufferLog() const
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


   void FramebufferObject::CollectAttachments()
   {
      const size_t attachmentsCount = std::count_if(mRenderTextures.begin(), mRenderTextures.end(), [](const auto& pair) { return (pair.first != GL_DEPTH_ATTACHMENT && pair.first != GL_DEPTH_STENCIL_ATTACHMENT); });

      if (attachmentsCount)
      {
         mFramebufferAttachments.reserve(attachmentsCount);
         for (const auto& attachmentToTexture : mRenderTextures)
         {
            if (attachmentToTexture.first != GL_DEPTH_ATTACHMENT && attachmentToTexture.first != GL_DEPTH_STENCIL_ATTACHMENT)
               mFramebufferAttachments.push_back(attachmentToTexture.first);
         }
      }
   }

   // should be called after CreateFramebuffer method was called
   void FramebufferObject::CreateRenderBuffer(const int32_t renderbufferDataType, const int32_t framebufferRenderbufferAttachment,
      const size_t screenResX, const size_t screenResY)
   {
      assert(mFramebufferId != std::numeric_limits<uint32_t>::max());

      glGenRenderbuffers(1, &mRenderBufferId);
      glBindRenderbuffer(GL_FRAMEBUFFER, mRenderBufferId);
      glRenderbufferStorage(GL_RENDERBUFFER, renderbufferDataType, screenResX, screenResY);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, framebufferRenderbufferAttachment, GL_RENDERBUFFER, mRenderBufferId);
   }

   void FramebufferObject::CreateRenderBuffer(const int32_t renderbufferDataType, const int32_t framebufferRenderbufferAttachment,
      const glm::ivec2& screenResolution)
   {
      assert(mFramebufferId != std::numeric_limits<uint32_t>::max());

      glGenRenderbuffers(1, &mRenderBufferId);
      glBindRenderbuffer(GL_RENDERBUFFER, mRenderBufferId);
      glRenderbufferStorage(GL_RENDERBUFFER, renderbufferDataType, screenResolution.x, screenResolution.y);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, framebufferRenderbufferAttachment, GL_RENDERBUFFER, mRenderBufferId);
   }

   void FramebufferObject::BindFramebuffer(bool bBindFramebuffer, bool enableAttachmentDrawBuffers) const
   {
      if (bBindFramebuffer)
      {
         glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferId);
      }

      if (enableAttachmentDrawBuffers)
      {
         if (mFramebufferAttachments.size() == 0)
         {
            glDrawBuffer(GL_NONE);
         }
         else
         {
            glDrawBuffers(mFramebufferAttachments.size(), mFramebufferAttachments.data());
         }
      }
   }

   void FramebufferObject::UnbindFramebuffer()
   {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }

   void FramebufferObject::CleanUp()
   {
      if (mFramebufferId != std::numeric_limits<uint32_t>::max())
         glDeleteFramebuffers(1, &mFramebufferId);

      if (mRenderBufferId != std::numeric_limits<uint32_t>::max())
         glDeleteRenderbuffers(1, &mRenderBufferId);
   }

}