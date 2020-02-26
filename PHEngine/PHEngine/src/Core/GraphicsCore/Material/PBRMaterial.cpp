#include "PBRMaterial.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/CommonCore/FolderManager.h"

namespace Graphics
{

   PBRMaterial::PBRMaterial(
      ITextureShared albedo,
      ITextureShared normalMap,
      ITextureShared metallicMap,
      ITextureShared roughnessMap,
      ITextureShared ambientOcclusionMap)
      : IMaterial("PBR Material", Common::FolderManager::GetInstance()->GetShadersPath() + "\\material_shaders\\PBRMaterial.glsl")
   {
         mProperties.emplace(std::make_pair("albedo", std::make_shared<TextureMaterialProperty>(albedo)));
         mProperties.emplace(std::make_pair("normalMap", std::make_shared<TextureMaterialProperty>(normalMap)));
         mProperties.emplace(std::make_pair("metallicMap", std::make_shared<TextureMaterialProperty>(metallicMap)));
         mProperties.emplace(std::make_pair("roughnessMap", std::make_shared<TextureMaterialProperty>(roughnessMap)));
         mProperties.emplace(std::make_pair("ambientOcclusionMap", std::make_shared<TextureMaterialProperty>(ambientOcclusionMap)));
   }

   PBRMaterial::~PBRMaterial()
   {
   }

}
