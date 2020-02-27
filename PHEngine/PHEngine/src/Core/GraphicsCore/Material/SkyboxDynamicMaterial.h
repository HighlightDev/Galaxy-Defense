#pragma once
#include "DynamicMaterial.h"

namespace Graphics
{

   class SkyboxDynamicMaterial :
      public DynamicMaterial
   {
      using ITextureShared = std::shared_ptr<ITexture>;

   public:

      SkyboxDynamicMaterial(ITextureShared dayTexture, ITextureShared nightTexure);
   };
}

