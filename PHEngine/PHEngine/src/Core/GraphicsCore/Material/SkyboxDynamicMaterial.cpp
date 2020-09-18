#include "SkyboxDynamicMaterial.h"

namespace Graphics
{

   SkyboxDynamicMaterial::SkyboxDynamicMaterial(ITextureShared dayTexture, ITextureShared nightTexure) 
      : DynamicMaterial("SkyboxDynamicMaterial", "SkyboxDayCycleMaterial.glsl")
   {
      PushMaterialProperty("dayTexture", std::make_shared<TextureMaterialProperty>(dayTexture));
      PushMaterialProperty("nightTexure", std::make_shared<TextureMaterialProperty>(nightTexure));
   }
}