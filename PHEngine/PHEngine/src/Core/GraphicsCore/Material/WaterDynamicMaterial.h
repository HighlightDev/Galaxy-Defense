#pragma once
#include "DynamicMaterial.h"

namespace Graphics
{
   class WaterDynamicMaterial :
      public DynamicMaterial
   {

      using ITextureShared = std::shared_ptr<ITexture>;

      std::shared_ptr<FloatMaterialProperty> moveFactorPropertyRef;
      std::shared_ptr<FloatMaterialProperty> strFactorPropertyRef;

   public:

      WaterDynamicMaterial(ITextureShared normalMap, ITextureShared distortion);

      void SetMoveFactor(const float moveFactor);

      void SetMoveStrengthFactor(const float strengthFactor);

      virtual void Tick(const float deltaTime) override;
   };

}

