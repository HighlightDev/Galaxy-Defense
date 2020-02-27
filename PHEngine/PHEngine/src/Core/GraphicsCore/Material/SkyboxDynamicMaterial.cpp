#include "SkyboxDynamicMaterial.h"
#include "Core/CommonCore/FolderManager.h"

namespace Graphics
{

   SkyboxDynamicMaterial::SkyboxDynamicMaterial(ITextureShared dayTexture, ITextureShared nightTexure) 
      : DynamicMaterial("SkyboxDynamicMaterial", Common::FolderManager::GetInstance()->GetShadersPath() + "\\material_shaders\\SkyboxDayCycleMaterial.glsl")
   {
      PushMaterialProperty("dayTexture", std::make_shared<TextureMaterialProperty>(dayTexture));
      PushMaterialProperty("nightTexure", std::make_shared<TextureMaterialProperty>(nightTexure));
   }
}