#pragma once
#include "IMaterial.h"
#include "Core/GameCore/ITickable.h"

namespace Graphics
{
   class DynamicMaterial 
      : public IMaterial
      , public ITickable
   {
   protected:

      using genericMaterialPropertyPair_t = std::pair<std::string, std::shared_ptr<MaterialProperty>>;

   public:

      DynamicMaterial(const std::string& materialName, const std::string& materialShaderName);

      virtual ~DynamicMaterial();

      virtual void Tick(const float deltaTime) = 0;
   };

}

