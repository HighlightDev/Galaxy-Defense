#pragma once
#include "IMaterial.h"
#include "Core/GameCore/ITickable.h"

namespace Graphics
{

   class DynamicMaterial 
      : public IMaterial
   {

   protected:

      using genericMaterialPropertyPair_t = std::pair<std::string, std::shared_ptr<MaterialProperty>>;

   public:

      DynamicMaterial(const std::string& materialName, const std::string& relativeMaterialShaderPath);

      ~DynamicMaterial();
   };

}

