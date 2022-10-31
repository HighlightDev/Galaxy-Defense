#include "BloomPostFxPass.h"

namespace Graphics
{

   BloomPostFxPass::BloomPostFxPass(const ViewPortInfo &viewPortInfo)
       : mBloomFramebuffer(std::make_unique<BloomFramebuffer>(viewPortInfo)),
         mViewPortInfo(viewPortInfo)
   {
   }

   BloomPostFxPass::~BloomPostFxPass()
   {
      if (mBloomFramebuffer)
      {
         mBloomFramebuffer->CleanUp();
      }
   }

   std::shared_ptr<ITexture> BloomPostFxPass::ExecutePostFx(const std::shared_ptr<ITexture> &sceneColorTexture)
   {
      return nullptr;
   }

   void BloomPostFxPass::CleanUp()
   {
      mBloomFramebuffer->CleanUp();
      mBloomFramebuffer.reset();
   }

}
