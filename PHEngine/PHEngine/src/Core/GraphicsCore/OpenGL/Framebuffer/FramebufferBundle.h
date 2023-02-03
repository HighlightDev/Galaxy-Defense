#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/vec4.hpp>
#include <gl/glew.h>

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "FramebufferObject.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

using namespace Graphics::Texture;

namespace Graphics
{
   class FramebufferBundle
   {
   public:
      FramebufferBundle();

      virtual ~FramebufferBundle();

      virtual void CleanUp() = 0;

      void UnbindFramebuffer(const GLbitfield clearFlag) const;

      virtual void CopyFramebufferDataToDefaultFramebuffer(const size_t srcX, const size_t srcY, const size_t srcResolutionX, const size_t srcResolutionY,
                                                           const size_t dstX, const size_t dstY, const size_t dstResolutionX, const size_t dstResolutionY,
                                                           const int32_t bufferBit);

      virtual void CopyFramebufferDataToDstFramebuffer(const std::shared_ptr<IFramebufferObject> &dstFramebuffer,
                                                       const size_t srcX, const size_t srcY, const size_t srcResolutionX, const size_t srcResolutionY,
                                                       const size_t dstX, const size_t dstY, const size_t dstResolutionX, const size_t dstResolutionY,
                                                       const int32_t bufferBit);

   protected:
      virtual void SetTextures() = 0;
      virtual void SetFramebuffers() = 0;
      virtual void SetRenderbuffers() = 0;
      void Init();

      void RenderToFBO(const FramebufferObject &fbo, bool bBindFramebuffer, size_t viewportX, size_t viewportY, size_t viewportWidth, size_t viewportHeight, GLbitfield clearFlag) const;
      void RenderToFBO(const FramebufferObject &fbo, bool bBindFramebuffer, const glm::ivec4 &viewport, GLbitfield clearFlag) const;
      void RenderToFBO(const FramebufferObject &fbo, bool bBindFramebuffer, const ViewPortInfo &viewport, GLbitfield clearFlag) const;
      void FramebufferClear(const FramebufferObject &fbo, const GLbitfield clearFlag);

      void CopySrcFramebufferDataToDefaultFramebufferImpl(const std::shared_ptr<IFramebufferObject> &srcFbo,
                                                          const size_t srcX, const size_t srcY, const size_t srcResolutionX, const size_t srcResolutionY,
                                                          const size_t dstX, const size_t dstY, const size_t dstResolutionX, const size_t dstResolutionY,
                                                          const int32_t bufferBit);

      void CopySrcFramebufferDataToDstFramebufferImpl(const std::shared_ptr<IFramebufferObject> &srcFbo,
                                                      const std::shared_ptr<IFramebufferObject> &dstFbo,
                                                      const size_t srcX, const size_t srcY, const size_t srcResolutionX, const size_t srcResolutionY,
                                                      const size_t dstX, const size_t dstY, const size_t dstResolutionX, const size_t dstResolutionY,
                                                      const int32_t bufferBit);

   private:
      void SetupViewport(size_t viewportX, size_t viewportY, size_t viewportWidth, size_t viewportHeight, GLbitfield clearFlag) const;
   };

}
