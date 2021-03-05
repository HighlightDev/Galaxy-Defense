#include "WaterDynamicMaterial.h"
#include "Core/IoCore/FolderManager.h"

namespace Graphics
{
   WaterDynamicMaterial::WaterDynamicMaterial(WaterDynamicMaterial::ITextureShared normalMap, WaterDynamicMaterial::ITextureShared distortion)
      : DynamicMaterial("WaterDynamicMaterial", "WaterMaterial.glsl")
   {
      mProperties.emplace(std::make_pair("normalMap", std::make_shared<TextureMaterialProperty>(normalMap)));
      mProperties.emplace(std::make_pair("distortion", std::make_shared<TextureMaterialProperty>(distortion)));

      moveFactorPropertyRef = std::make_shared<FloatMaterialProperty>(0.0f);
      strFactorPropertyRef = std::make_shared<FloatMaterialProperty>(0.0f);

      mProperties.emplace(DynamicMaterial::genericMaterialPropertyPair_t("moveFactor", moveFactorPropertyRef));
      mProperties.emplace(DynamicMaterial::genericMaterialPropertyPair_t("strengthFactor", strFactorPropertyRef));
   }

   void WaterDynamicMaterial::SetMoveFactor(const float moveFactor)
   {
      moveFactorPropertyRef->SetValue(moveFactor);
   }

   void WaterDynamicMaterial::SetMoveStrengthFactor(const float strengthFactor)
   {
      strFactorPropertyRef->SetValue(strengthFactor);
   }

   void WaterDynamicMaterial::Tick(const float deltaTime)
   {

   }
}
