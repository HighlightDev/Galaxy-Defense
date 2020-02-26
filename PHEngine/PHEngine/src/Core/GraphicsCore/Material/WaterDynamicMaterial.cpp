#include "WaterDynamicMaterial.h"
#include "Core/CommonCore/FolderManager.h"

#define STUB "STUB"

namespace Graphics
{
   WaterDynamicMaterial::WaterDynamicMaterial()
      : DynamicMaterial("WaterDynamicMaterial", Common::FolderManager::GetInstance()->GetShadersPath() + "\\material_shaders\\WaterMaterial.glsl")
   {
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
}
