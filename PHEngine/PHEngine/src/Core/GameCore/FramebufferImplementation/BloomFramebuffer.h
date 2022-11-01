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

         const ViewPortInfo mViewPortInfo;

         RenderTarget m_color1;
         RenderTarget m_color2;

         std::shared_ptr<FramebufferObject> mColor1Framebuffer;
         std::shared_ptr<FramebufferObject> mColor2Framebuffer;

      public:
         // Buffer should be recreated when window size was changed
         BloomFramebuffer(const ViewPortInfo &viewPortInfo);

         virtual ~BloomFramebuffer();

         virtual void SetTextures() override;

         virtual void SetFramebuffers() override;

         virtual void SetRenderbuffers() override;

         virtual void CleanUp() override;

         void BindColor1Framebuffer();
         void BindColor2Framebuffer();

         void BindColor1Texture(int32_t slot);
         void BindColor2Texture(int32_t slot);

         std::shared_ptr<ITexture> GetColor1Texture() const;
         std::shared_ptr<ITexture> GetColor2Texture() const;

      private:
         void DestroyBloomFramebuffer();
      };
   }
}
