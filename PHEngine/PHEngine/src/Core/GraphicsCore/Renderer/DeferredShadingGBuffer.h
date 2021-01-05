#pragma once
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"
#include "Core/GraphicsCore/OpenGL/Framebuffer/Framebuffer.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

using namespace Graphics::Texture;
using namespace Resources;

namespace Graphics
{

   class DeferredShadingGBuffer
      : public Framebuffer
   {
      using RenderTarget = std::shared_ptr<ITexture>;

      const ViewPortInfo mViewPortInfo;

      RenderTarget m_depthBuffer;
      RenderTarget m_positionBuffer;
      RenderTarget m_normalBuffer;
      RenderTarget m_albedoWithSpecularBuffer;

   public:

      // Buffer should be recreated when window size was changed
      DeferredShadingGBuffer(const ViewPortInfo& viewPortInfo);

      virtual ~DeferredShadingGBuffer();

      virtual void SetTextures() override;

      virtual void SetFramebuffers() override;

      virtual void SetRenderbuffers() override;

      virtual void CleanUp() override;

      void BindDeferredGBuffer();

      void UnbindDeferredGBuffer();

      void BindDepthTexture(int32_t slot);

      void BindPositionTexture(int32_t slot);

      void BindNormalTexture(int32_t slot);

      void BindAlbedoWithSpecularTexture(int32_t slot);

      void CopyFramebufferData(size_t srcX, size_t srcY, size_t srcResolutionX, size_t srcResolutionY,
         size_t dstX, size_t dstY, size_t dstResolutionX, size_t dstResolutionY, int32_t bufferBit);

   private:

      void DestroyGBuffer();
   };
}

