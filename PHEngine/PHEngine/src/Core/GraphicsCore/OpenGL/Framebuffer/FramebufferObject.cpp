#include "FramebufferObject.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/LoggerExtension.h"

#include <algorithm>

using namespace EngineCore;

namespace Graphics
{

   FramebufferObject::FramebufferObject()
       : mFramebufferId(std::numeric_limits<uint32_t>::max()),
         mRenderBufferId(std::numeric_limits<uint32_t>::max()),
         mRenderTextures()
   {
   }

   FramebufferObject::~FramebufferObject()
   {
   }

   void FramebufferObject::AddRenderTexture(const uint32_t framebufferAttachement, const std::shared_ptr<ITexture> &renderTexture)
   {
      assert(mRenderTextures.count(framebufferAttachement) == 0);
      mRenderTextures[framebufferAttachement] = renderTexture;
   }

   void FramebufferObject::ReassignRenderTexture(const uint32_t framebufferAttachement, const std::shared_ptr<ITexture> &renderTexture)
   {
      assert(mRenderTextures.count(framebufferAttachement) > 0);
      mRenderTextures[framebufferAttachement] = renderTexture;
   }

   // should be called after all render textures are attached
   void FramebufferObject::CreateFramebuffer()
   {
      assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"));
      glGenFramebuffers(1, &mFramebufferId);
      glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferId);

      for (const auto &[attachment, textureSp] : mRenderTextures)
      {
         if (textureSp->GetTextureType() == eTextureType::TEXTURE_2D)
         {
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, textureSp->GetTextureDescriptor(), 0);
         }
         else if (textureSp->GetTextureType() == eTextureType::TEXTURE_CUBE)
         {
            glFramebufferTexture(GL_FRAMEBUFFER, attachment, textureSp->GetTextureDescriptor(), 0);
         }
      }

      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      CollectAttachments();
   }

   void FramebufferObject::RebindFramebufferTextures()
   {
      assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"));
      glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferId);
      for (const auto &[attachment, textureSp] : mRenderTextures)
      {
         if (textureSp->GetTextureType() == eTextureType::TEXTURE_2D)
         {
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, textureSp->GetTextureDescriptor(), 0);
         }
         else if (textureSp->GetTextureType() == eTextureType::TEXTURE_CUBE)
         {
            glFramebufferTexture(GL_FRAMEBUFFER, attachment, textureSp->GetTextureDescriptor(), 0);
         }
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }

   void FramebufferObject::CheckErrors()
   {
      glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferId);
      GetFramebufferErrorCode();
   }

   GLenum FramebufferObject::GetFramebufferErrorCode() const
   {
      GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (GL_FRAMEBUFFER_COMPLETE != result)
      {
         LogInfo(GetFramebufferLog());
         assert(false); // this code should not be reached
      }
      return result;
   }

   std::string FramebufferObject::GetFramebufferLog() const
   {
      GLenum errorCode = GetFramebufferErrorCode();
      switch (errorCode)
      {
      case GL_FRAMEBUFFER_COMPLETE:
      {
         return "Framebuffer : complete.";
      }
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      {
         return "Framebuffer : Not all framebuffer attachment points are framebuffer attachment complete.";
      }
      case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
      {
         return "Framebuffer : Not all attached images have the same width and height.";
      }
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
      {
         return "Framebuffer : Each draw buffer must specify color attachment points that have images attached or must be GL_NONE.";
      }
      case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
      {
         return "Framebuffer : Incomplete formats";
      }
      case GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_EXT:
      {
         return "Framebuffer : Incomplete layer count";
      }
      case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
      {
         return "Framebuffer : All attachments must be layered attachments.";
      }
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      {
         return "Framebuffer : No images are attached to the framebuffer.";
      }
      case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
      {
         return "Framebuffer : All images must have the same number of multisample samples.";
      }
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
      {
         return "Framebuffer : ReadBuffer must specify an attachment point that has an image attached. ";
      }
      case GL_FRAMEBUFFER_UNDEFINED:
      {
         return "Framebuffer : FBO object number 0 is bound";
      }
      case GL_FRAMEBUFFER_UNSUPPORTED:
      {
         return "Framebuffer : The combination of internal formats of the attached images violates an implementation-dependent set of restrictions.";
      }
      default:
         return "Framebuffer : Undefined error.";
      }
   }

   void FramebufferObject::CollectAttachments()
   {
      const size_t attachmentsCount = std::count_if(mRenderTextures.begin(), mRenderTextures.end(), [](const auto &pair)
                                                    { return (pair.first != GL_DEPTH_ATTACHMENT && pair.first != GL_DEPTH_STENCIL_ATTACHMENT); });

      if (attachmentsCount)
      {
         mFramebufferAttachments.reserve(attachmentsCount);
         for (const auto &attachmentToTexture : mRenderTextures)
         {
            if (attachmentToTexture.first != GL_DEPTH_ATTACHMENT && attachmentToTexture.first != GL_DEPTH_STENCIL_ATTACHMENT)
               mFramebufferAttachments.push_back(attachmentToTexture.first);
         }
         std::sort(mFramebufferAttachments.begin(), mFramebufferAttachments.end(), [](const auto &left, const auto &right) -> bool
                   { return left < right; });
      }
   }

   // should be called after CreateFramebuffer method was called
   void FramebufferObject::CreateRenderBuffer(const int32_t renderbufferDataType, const int32_t framebufferRenderbufferAttachment,
                                              const size_t screenResX, const size_t screenResY)
   {
      assert(mFramebufferId != std::numeric_limits<uint32_t>::max());
      assert(mRenderBufferId == std::numeric_limits<uint32_t>::max());
      assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"));

      glGenRenderbuffers(1, &mRenderBufferId);
      glBindRenderbuffer(GL_FRAMEBUFFER, mRenderBufferId);
      glRenderbufferStorage(GL_RENDERBUFFER, renderbufferDataType, screenResX, screenResY);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, framebufferRenderbufferAttachment, GL_RENDERBUFFER, mRenderBufferId);
   }

   void FramebufferObject::CreateRenderBuffer(const int32_t renderbufferDataType, const int32_t framebufferRenderbufferAttachment,
                                              const glm::ivec2 &screenResolution)
   {
      assert(mFramebufferId != std::numeric_limits<uint32_t>::max());
      assert(mRenderBufferId == std::numeric_limits<uint32_t>::max());
      assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"));

      glGenRenderbuffers(1, &mRenderBufferId);
      glBindRenderbuffer(GL_RENDERBUFFER, mRenderBufferId);
      glRenderbufferStorage(GL_RENDERBUFFER, renderbufferDataType, screenResolution.x, screenResolution.y);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, framebufferRenderbufferAttachment, GL_RENDERBUFFER, mRenderBufferId);
   }

   void FramebufferObject::ResizeRenderBufferStorage(const int32_t renderbufferDataType, const glm::ivec2 &screenResolution)
   {
      assert(mRenderBufferId != std::numeric_limits<uint32_t>::max());
      glBindRenderbuffer(GL_RENDERBUFFER, mRenderBufferId);
      glRenderbufferStorage(GL_RENDERBUFFER, renderbufferDataType, screenResolution.x, screenResolution.y);
   }

   void FramebufferObject::BindFramebuffer(uint32_t framebufferTarget, bool bBindFramebuffer, bool enableAttachmentDrawBuffers) const
   {
      if (bBindFramebuffer)
      {
         glBindFramebuffer(framebufferTarget, mFramebufferId);
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

   void FramebufferObject::BindFramebufferAsReadTarget()
   {
      glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebufferId);
   }

   void FramebufferObject::BindFramebufferAsDrawTarget()
   {
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebufferId);
   }

   void FramebufferObject::BindFramebufferAsReadDrawTarget()
   {
      glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferId);
   }

}