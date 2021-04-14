#pragma once
#include "IMaterial.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GraphicsCore/Material/DynamicFloatMaterialProperty.h"

namespace Graphics
{
   class DynamicMaterial 
      : public IMaterial
      , public ITickable
   {
   protected:

      std::vector<std::shared_ptr<DynamicFloatMaterialProperty>> mDynamicProperties;

   public:

      DynamicMaterial(const std::string& materialName, const std::string& materialShaderName);

      virtual ~DynamicMaterial();

      virtual eMaterialType GetMaterialType() const override;

      virtual void Tick(const float deltaTime);

      void PushDynamicProperty(std::shared_ptr<DynamicFloatMaterialProperty> dynamicProperty);

      std::shared_ptr<DynamicFloatMaterialProperty> TryGetDynamicPropertyByName(const std::string& propertyName) const;

      std::shared_ptr<MaterialProperty> TryGetAnyMaterialPropertyByName(const std::string& propertyName) const;
      
   };

}

