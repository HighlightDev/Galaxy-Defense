#pragma once
#include "DynamicMaterial.h"

namespace Graphics
{
   class WaterDynamicMaterial :
      public DynamicMaterial
   {

      std::shared_ptr<FloatMaterialProperty> moveFactorPropertyRef;
      std::shared_ptr<FloatMaterialProperty> strFactorPropertyRef;

   public:

      WaterDynamicMaterial();

      void SetMoveFactor(const float moveFactor);

      void SetMoveStrengthFactor(const float strengthFactor);
   };

}

