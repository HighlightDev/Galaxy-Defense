#pragma once
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"
#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferBundle.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferObject.h"

using namespace Graphics;
using namespace Graphics::Texture;
using namespace Resources;

namespace EngineCore
{
   namespace FramebufferImpl
   {
      class BloomFramebuffer
          : public FramebufferBundle
      {
         using RenderTarget = std::shared_ptr<ITexture>;

      private:
         ViewPortInfo mFullScreenResolutionViewPortInfo;
         ViewPortInfo mShrinkedResolutionViewPortInfo;

         RenderTarget m_color1;
         RenderTarget m_color2;

         std::shared_ptr<FramebufferObject> mColor1Framebuffer;
         std::shared_ptr<FramebufferObject> mColor2Framebuffer;

      public:
         // Buffer should be recreated when window size was changed
         BloomFramebuffer(const ViewPortInfo &viewPortInfo);

         ~BloomFramebuffer() override;

         void SetTextures() override;

         void SetFramebuffers() override;

         void SetRenderbuffers() override;

         void CleanUp() override;

         void CleanColor1Framebuffer(const GLint clearBit);

         void BindColor1Framebuffer(const GLint clearBitFlag);
         void BindColor2Framebuffer();

         void BindColor1Texture(int32_t slot);
         void BindColor2Texture(int32_t slot);

         std::shared_ptr<ITexture> GetColor1Texture() const;
         std::shared_ptr<ITexture> GetColor2Texture() const;

         std::shared_ptr<IFramebufferObject> GetColor1FramebufferObjectInstance() const;

         ViewPortInfo GetFullScreenResolutionViewPortInfo() const;
         ViewPortInfo GetShrinkedResolutionViewPortInfo() const;

      private:
         void DestroyBloomFramebuffer();
      };
   }
}
