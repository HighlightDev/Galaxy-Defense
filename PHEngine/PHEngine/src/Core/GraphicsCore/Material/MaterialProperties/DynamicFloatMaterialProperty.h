#pragma once
#include "MaterialProperty.h"
#include "Core/GraphicsCore/Material/DynamicMaterialOperations/MaterialEnterNode.h"

#include <algorithm>
#include <limits>
#include <glm/vec2.hpp>

#undef max
#undef min

namespace Graphics {

   struct DynamicFloatMaterialProperty
      : public MaterialProperty
   {
      using MaterialPropertyValueType = std::shared_ptr<MaterialEnterNode>;

   private:

      MaterialPropertyValueType mDynamicOperationStartNode;

      std::vector<std::shared_ptr<MaterialProperty>> mInternalDynamicMaterialProperties;

      bool bValueIncremental;

      glm::vec2 mRangeMinMax;

      float mCachedValue;
      
   public:

      DynamicFloatMaterialProperty(MaterialPropertyValueType startNode, const std::string& propertyName)
         : MaterialProperty(propertyName)
         , mDynamicOperationStartNode(startNode)
         , bValueIncremental(false)
         , mRangeMinMax(std::numeric_limits<float>::min(), std::numeric_limits<float>::max())
         , mCachedValue(0.0f)
      {
      }

      DynamicFloatMaterialProperty(const std::string& propertyName)
         : MaterialProperty(propertyName)
         , mDynamicOperationStartNode()
         , bValueIncremental(false)
         , mRangeMinMax(std::numeric_limits<float>::min(), std::numeric_limits<float>::max())
         , mCachedValue(0.0f)
      {
      }

      void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override { assert(false); } // this code should not be called, use proxy on render thread part 

      eMaterialPropertyType GetPropertyType() const override
      {
         return MaterialProperty::eMaterialPropertyType::FLOAT_PROPERTY;
      }

      void SetValue(MaterialPropertyValueType startNode) {
         mDynamicOperationStartNode = startNode;
      }

      float GetValue() 
      {
         auto value = mDynamicOperationStartNode->GetValue();

         /*If incremental - add new value to previous value*/
         if (bValueIncremental) {
           value += mCachedValue;
         }
         
         /*Range*/
         if (value < mRangeMinMax.x)
         {
            value = mRangeMinMax.y - (mRangeMinMax.x - std::abs(value));
         }
         else if (value > mRangeMinMax.y)
         {
            value = std::fmod(value, mRangeMinMax.y);
         }

         mCachedValue = value;

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

      void SetRange(const glm::vec2& range) {
         mRangeMinMax = range;
         mCachedValue = mRangeMinMax.x;
      }

      glm::vec2 GetRange() const {
         return mRangeMinMax;
      }

      void SetIsValueIncremental(bool isIncremental) {
         bValueIncremental = isIncremental;
      }

      bool IsValueIncremental() const {
         return bValueIncremental;
      }

   };
}

