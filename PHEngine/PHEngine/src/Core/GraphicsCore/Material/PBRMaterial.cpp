#include "PBRMaterial.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/IoCore/FolderManager.h"

namespace Graphics
{

   PBRMaterial::PBRMaterial(
      ITextureShared albedo,
      ITextureShared normalMap,
      ITextureShared metallicMap,
      ITextureShared roughnessMap,
      ITextureShared ambientOcclusionMap,
      const float uvScale)
      : IMaterial("PBR Material", IO::FolderManager::GetInstance()->GetShadersPath() + "\\material_shaders\\PBRMaterial.glsl")
   {
      PushMaterialProperty("albedo", std::make_shared<TextureMaterialProperty>(albedo));
      PushMaterialProperty("normalMap", std::make_shared<TextureMaterialProperty>(normalMap));
      PushMaterialProperty("metallicMap", std::make_shared<TextureMaterialProperty>(metallicMap));
      PushMaterialProperty("roughnessMap", std::make_shared<TextureMaterialProperty>(roughnessMap));
      PushMaterialProperty("ambientOcclusionMap", std::make_shared<TextureMaterialProperty>(ambientOcclusionMap));
      PushMaterialProperty("uvScale", std::make_shared<FloatMaterialProperty>(uvScale));
   }

   PBRMaterial::~PBRMaterial()
   {
   }

}
