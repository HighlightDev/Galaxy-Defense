#pragma once
#include "MaterialProperty.h"
#include "Core/GraphicsCore/Material/DynamicMaterialOperations/MaterialOperation.h"

#include <algorithm>

namespace Graphics {

   struct DynamicFloatMaterialProperty
      : public MaterialProperty
   {
      using MaterialPropertyValueType = std::shared_ptr<MaterialStartNode>;

   private:

      MaterialPropertyValueType mDynamicOperationStartNode;

      std::vector<std::shared_ptr<MaterialProperty>> mInternalDynamicMaterialProperties;

   public:

      DynamicFloatMaterialProperty(MaterialPropertyValueType startNode, const std::string& propertyName)
         : MaterialProperty(propertyName)
         , mDynamicOperationStartNode(startNode)
      {
      }

      DynamicFloatMaterialProperty(const std::string& propertyName)
         : MaterialProperty(propertyName)
         , mDynamicOperationStartNode()
      {
      }

      virtual void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override { assert(false); } // this code should not be called, use proxy on render thread part 

      virtual eMaterialPropertyType GetPropertyType() const override
      {
         return MaterialProperty::eMaterialPropertyType::FLOAT_PROPERTY;
      }

      void SetValue(MaterialPropertyValueType startNode) {
         mDynamicOperationStartNode = startNode;
      }

      inline float GetValue() const {
         auto value = mDynamicOperationStartNode->GetValue();
         return value;
      }

      void SetInternalDynamicMaterialProperties(std::vector<std::shared_ptr<MaterialProperty>>&& internalDynamicMaterialProperties)
      {
         mInternalDynamicMaterialProperties = internalDynamicMaterialProperties;
      }

      std::shared_ptr<MaterialProperty> TryGetInternalMaterialPropertyByName(const std::string& propertyName) const 
      {
         auto propertyIt = std::find_if(mInternalDynamicMaterialProperties.begin(), mInternalDynamicMaterialProperties.end(),
            [&](const auto& dynamicProperty) { return propertyName == dynamicProperty->GetPropertyName(); });

         if (propertyIt != mInternalDynamicMaterialProperties.end())
            return *propertyIt;
         else
            return nullptr;
      }

   };
}

