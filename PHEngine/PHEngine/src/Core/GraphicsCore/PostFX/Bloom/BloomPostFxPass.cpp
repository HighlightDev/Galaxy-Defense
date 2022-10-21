#include "BloomPostFxPass.h"

namespace Graphics 
{

   BloomPostFxPass::BloomPostFxPass()
   {
   }

   BloomPostFxPass::~BloomPostFxPass()
   {
   }

   std::shared_ptr<ITexture> BloomPostFxPass::ExecutePostFx(const std::shared_ptr<ITexture>& sceneColorTexture)
   {
      return nullptr;
   }

}
