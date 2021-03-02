#pragma once
#include "IMaterial.h"
#include "Core/GameCore/ITickable.h"

namespace Graphics
{
   // stub
   struct MaterialProxy {};

   class DynamicMaterial 
      : public IMaterial
      , public ITickable
   {

   public:

      DynamicMaterial(const std::string& materialName, const std::string& materialShaderName);

      virtual ~DynamicMaterial();

      virtual void Tick(const float deltaTime) = 0;

      virtual std::shared_ptr<MaterialProxy> GetMaterialProxy() const = 0;
   };

}

