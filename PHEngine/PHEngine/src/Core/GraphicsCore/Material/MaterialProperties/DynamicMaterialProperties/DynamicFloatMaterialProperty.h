#pragma once
#include "DynamicMaterialProperty.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/FloatMaterialProperty.h"
#include "Core/GraphicsCore/Material/DynamicMaterialOperations/MaterialEnterNode.h"

#include <algorithm>
#include <limits>
#include <any>
#include <glm/vec2.hpp>

#undef max
#undef min

namespace Graphics
{

   struct DynamicFloatMaterialProperty
       : public DynamicMaterialProperty
   {
      using MaterialPropertyValueType = std::shared_ptr<MaterialEnterNode>;

   private:
      MaterialPropertyValueType mDynamicOperationStartNode;

      bool bValueIncremental;

      bool mRangeIsSet{false};
      glm::vec2 mRangeMinMax;

      float mCachedValue;

   public:
      DynamicFloatMaterialProperty(MaterialPropertyValueType startNode, const std::string &propertyName)
          : DynamicMaterialProperty(propertyName),
            mDynamicOperationStartNode(startNode),
            bValueIncremental(false),
            mRangeMinMax(std::numeric_limits<float>::min(), std::numeric_limits<float>::max()),
            mCachedValue(0.0f)
      {
      }

      DynamicFloatMaterialProperty(const std::string &propertyName)
          : DynamicMaterialProperty(propertyName),
            mDynamicOperationStartNode(),
            bValueIncremental(false),
            mRangeMinMax(std::numeric_limits<float>::min(), std::numeric_limits<float>::max()),
            mCachedValue(0.0f)
      {
      }

      void SetValueToUniform(const Uniform &uniform, const int32_t propertyIndex) const override { assert(false); } // this code should not be called, use proxy on render thread part

      eDynamicMaterialPropertyType GetPropertyType() const override
      {
         return DynamicMaterialProperty::eDynamicMaterialPropertyType::FloatProperty;
      }

      void UpdateStaticPropertyWithDynamicValue(const std::shared_ptr<MaterialProperty> &staticProperty) override
      {
         assert(MaterialProperty::eMaterialPropertyType::FLOAT_PROPERTY == staticProperty->GetPropertyType());
         const auto &staticFloatMaterialProperty = std::static_pointer_cast<FloatMaterialProperty>(staticProperty);
         staticFloatMaterialProperty->SetValue(GetValue());
      }

      void SetValue(MaterialPropertyValueType startNode)
      {
         mDynamicOperationStartNode = startNode;
      }

      float GetValue()
      {
         assert(mDynamicOperationStartNode->GetMaterialPropertyType() == MaterialNode::eMaterialPropertyType::FLOAT);
         auto value = std::any_cast<float>(mDynamicOperationStartNode->GetValue());

         /*If incremental - add new value to previous value*/
         if (bValueIncremental)
         {
            value += mCachedValue;
         }

         if (mRangeIsSet)
         {
            /*Range*/
            if (value < mRangeMinMax.x)
            {
               value = mRangeMinMax.y - (mRangeMinMax.x - std::abs(value));
            }
            else if (value > mRangeMinMax.y)
            {
               value = std::fmod(value, mRangeMinMax.y);
            }
         }

         mCachedValue = value;

         return value;
      }

      void SetRange(const glm::vec2 &range)
      {
         mRangeIsSet = true;
         mRangeMinMax = range;
      }

      glm::vec2 GetRange() const
      {
         return mRangeMinMax;
      }

      void SetIsValueIncremental(bool isIncremental)
      {
         bValueIncremental = isIncremental;
      }

      bool IsValueIncremental() const
      {
         return bValueIncremental;
      }
   };
}
