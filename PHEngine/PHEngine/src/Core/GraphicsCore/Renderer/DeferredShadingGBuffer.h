#pragma once
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"
#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferBundle.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferObject.h"

using namespace Graphics::Texture;
using namespace Resources;

namespace Graphics
{

   class DeferredShadingGBuffer
       : public FramebufferBundle
   {
      using RenderTarget = std::shared_ptr<ITexture>;

      const ViewPortInfo mViewPortInfo;

      RenderTarget m_depthBuffer;
      RenderTarget m_positionBuffer;
      RenderTarget m_normalBuffer;
      RenderTarget m_albedoBuffer;
      RenderTarget m_metallicRoughnessBuffer;

      std::shared_ptr<FramebufferObject> mFramebuffer;

   public:
      // Buffer should be recreated when window size was changed
      DeferredShadingGBuffer(const ViewPortInfo &viewPortInfo);

      ~DeferredShadingGBuffer() override;

      void SetTextures() override;

      void SetFramebuffers() override;

      void SetRenderbuffers() override;

      void CleanUp() override;

      void BindDeferredGBuffer();

      void UnbindDeferredGBuffer();

      void BindDepthTexture(int32_t slot);

      void BindPositionTexture(int32_t slot);

      void BindNormalTexture(int32_t slot);

      void BindAlbedoTexture(int32_t slot);

      void BindMetallicRoughnessTexture(int32_t slot);

      std::shared_ptr<IFramebufferObject> GetFramebufferObjectInstance() const;

      void CopyFramebufferDataToDefaultFramebuffer(const size_t srcX, const size_t srcY, const size_t srcResolutionX, const size_t srcResolutionY,
                                                   const size_t dstX, const size_t dstY, const size_t dstResolutionX, const size_t dstResolutionY,
                                                   const int32_t bufferBit) override;

      void CopyFramebufferDataToDstFramebuffer(const std::shared_ptr<IFramebufferObject> &framebufferObjectInstance,
                                               const size_t srcX, const size_t srcY, const size_t srcResolutionX, const size_t srcResolutionY,
                                               const size_t dstX, const size_t dstY, const size_t dstResolutionX, const size_t dstResolutionY,
                                               const int32_t bufferBit) override;

   private:
      void DestroyGBuffer();
   };
}
