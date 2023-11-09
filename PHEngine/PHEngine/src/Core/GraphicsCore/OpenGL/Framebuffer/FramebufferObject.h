#pragma once
#include <cstddef>
#include <memory>
#include <unordered_map>

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "IFramebufferObject.h"

using namespace Graphics::Texture;

namespace Graphics
{

   class FramebufferObject
       : public IFramebufferObject
   {
   private:
      uint32_t mFramebufferId;
      uint32_t mRenderBufferId;

      std::unordered_map<uint32_t /*FramebufferBundle attachment*/, std::shared_ptr<ITexture>> mRenderTextures;

      std::vector<uint32_t> mFramebufferAttachments;

   public:
      FramebufferObject();

      ~FramebufferObject();

      GLenum GetFramebufferErrorCode() const;

      std::string GetFramebufferLog() const;

      void AddRenderTexture(const uint32_t framebufferAttachement, const std::shared_ptr<ITexture>& renderTexture);

      void ReassignRenderTexture(const uint32_t framebufferAttachement, const std::shared_ptr<ITexture>& renderTexture);

      // should be called after all render textures are added
      void CreateFramebuffer();

      void RebindFramebufferTextures();

      // should be called after CreateFramebuffer method was called
      void CreateRenderBuffer(const int32_t renderbufferDataType, const int32_t framebufferRenderbufferAttachment,
                              const size_t screenResX, const size_t screenResY);

      void CreateRenderBuffer(const int32_t renderbufferDataType, const int32_t framebufferRenderbufferAttachment,
                              const glm::ivec2 &screenResolution);

      void ResizeRenderBufferStorage(const int32_t renderbufferDataType, const glm::ivec2& screenResolution);

      void BindFramebuffer(uint32_t framebufferTarget, bool bBindFramebuffer, bool enableAttachmentDrawBuffers = true) const;

      void CheckErrors();

      void UnbindFramebuffer();

      void CleanUp();

      void BindFramebufferAsReadTarget() override;

      void BindFramebufferAsDrawTarget() override;

      void BindFramebufferAsReadDrawTarget() override;

   private:
      void CollectAttachments();
   };

}
