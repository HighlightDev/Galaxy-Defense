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

      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      CollectAttachments();
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
      glBindRenderbuffer(GL_FRAMEBUFFER, mRenderBufferId);
      glRenderbufferStorage(GL_RENDERBUFFER, renderbufferDataType, screenResolution.x, screenResolution.y);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, framebufferRenderbufferAttachment, GL_RENDERBUFFER, mRenderBufferId);
   }

   void FramebufferObject::BindFramebuffer(bool enableAttachmentDrawBuffers) const
   {
      glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferId);

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