#include "DynamicMaterial.h"
#include "Core/GraphicsCore/Material/FloatMaterialProperty.h"

#include <algorithm>

namespace Graphics
{
   DynamicMaterial::DynamicMaterial(const std::string& materialName, const std::string& materialShaderName)
      : IMaterial(materialName, materialShaderName)
   {
   }

   DynamicMaterial::~DynamicMaterial()
   {
   }

   IMaterial::eMaterialType DynamicMaterial::GetMaterialType() const {
      return IMaterial::eMaterialType::DYNAMIC;
   }

   void DynamicMaterial::Tick(const float deltaTime) 
   {
      for (auto dynProp : mDynamicProperties) {

         const float value = dynProp->GetValue();

      }
   }

   void DynamicMaterial::PushDynamicProperty(std::shared_ptr<DynamicFloatMaterialProperty> dynamicProperty) {

      const auto& propertyName = dynamicProperty->GetPropertyName();

      auto propertyIt = std::find_if(mDynamicProperties.begin(), mDynamicProperties.end(),
         [&](const auto& dynamicProperty) { return propertyName == dynamicProperty->GetPropertyName(); });
      assert(propertyIt == mDynamicProperties.end());

      std::shared_ptr<FloatMaterialProperty> proxyProperty = std::make_shared<FloatMaterialProperty>(propertyName);
      PushMaterialProperty(proxyProperty);
      
      mDynamicProperties.emplace_back(std::move(dynamicProperty));
   }

   std::shared_ptr<MaterialProperty> DynamicMaterial::TryGetAnyMaterialPropertyByName(const std::string& propertyName) const
   {
      std::shared_ptr<MaterialProperty> result = nullptr;

      for (const auto& dynamicProperty : mDynamicProperties) {
         auto property = dynamicProperty->TryGetInternalMaterialPropertyByName(propertyName);
         if (property)
         {
            result = property;
            break;
         }
      }

      if (!result)
      {
         result = GetMaterialPropertyByName(propertyName);
      }

      return result;
   }

   std::shared_ptr<DynamicFloatMaterialProperty> DynamicMaterial::TryGetDynamicPropertyByName(const std::string& propertyName) const
   {
      auto propertyIt = std::find_if(mDynamicProperties.begin(), mDynamicProperties.end(),
         [&](const auto& dynamicProperty) { return propertyName == dynamicProperty->GetPropertyName(); });
      if (propertyIt != mDynamicProperties.end()) {
         return *propertyIt;
      }

      return nullptr;
   }
}