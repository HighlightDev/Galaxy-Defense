#pragma once
#include "DynamicMaterialProperty.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/Vec2MaterialProperty.h"
#include "Core/GraphicsCore/Material/DynamicMaterialOperations/MaterialEnterNode.h"

#include <algorithm>
#include <limits>
#include <any>
#include <glm/vec2.hpp>

#undef max
#undef min

namespace Graphics
{

   struct DynamicVec2MaterialProperty
       : public DynamicMaterialProperty
   {
      using MaterialPropertyValueType = std::shared_ptr<MaterialEnterNode>;

   private:
      MaterialPropertyValueType mDynamicOperationStartNode;

      bool bValueIncremental;

      bool mRangeIsSet{false};
      glm::vec2 mRangeMinMax;

      glm::vec2 mCachedValue;

   public:
      DynamicVec2MaterialProperty(MaterialPropertyValueType startNode, const std::string &propertyName)
          : DynamicMaterialProperty(propertyName),
            mDynamicOperationStartNode(startNode),
            bValueIncremental(false),
            mRangeMinMax(std::numeric_limits<float>::min(), std::numeric_limits<float>::max()),
            mCachedValue()
      {
      }

      DynamicVec2MaterialProperty(const std::string &propertyName)
          : DynamicMaterialProperty(propertyName),
            mDynamicOperationStartNode(),
            bValueIncremental(false),
            mRangeMinMax(std::numeric_limits<float>::min(), std::numeric_limits<float>::max()),
            mCachedValue()
      {
      }

      eDynamicMaterialPropertyType GetPropertyType() const override
      {
         return DynamicMaterialProperty::eDynamicMaterialPropertyType::Vec2Property;
      }

      void UpdateStaticPropertyWithDynamicValue(const std::shared_ptr<MaterialProperty> &staticProperty) override
      {
         assert(MaterialProperty::eMaterialPropertyType::VEC2_PROPERTY == staticProperty->GetPropertyType());
         const auto &staticVec2MaterialProperty = std::static_pointer_cast<Vec2MaterialProperty>(staticProperty);
         staticVec2MaterialProperty->SetValue(GetValue());
      }

      void SetValue(MaterialPropertyValueType startNode)
      {
         mDynamicOperationStartNode = startNode;
      }

      glm::vec2 GetValue()
      {
         assert(mDynamicOperationStartNode->GetMaterialPropertyType() == MaterialNode::eMaterialPropertyType::VEC2);
         auto value = std::any_cast<glm::vec2>(mDynamicOperationStartNode->GetValue());

         /*If incremental - add new value to previous value*/
         if (bValueIncremental)
         {
            value += mCachedValue;
         }

         if (mRangeIsSet)
         {
            /*Range*/
            if (value.x < mRangeMinMax.x)
            {
               value.x = mRangeMinMax.y - (mRangeMinMax.x - std::abs(value.x));
            }
            else if (value.x > mRangeMinMax.y)
            {
               value.x = std::fmod(value.x, mRangeMinMax.y);
            }

            if (value.y < mRangeMinMax.x)
            {
               value.y = mRangeMinMax.y - (mRangeMinMax.x - std::abs(value.y));
            }
            else if (value.y > mRangeMinMax.y)
            {
               value.y = std::fmod(value.y, mRangeMinMax.y);
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

      void SetIsValueIncremental(const bool isIncremental)
      {
         bValueIncremental = isIncremental;
      }

      bool IsValueIncremental() const
      {
         return bValueIncremental;
      }
   };
}
