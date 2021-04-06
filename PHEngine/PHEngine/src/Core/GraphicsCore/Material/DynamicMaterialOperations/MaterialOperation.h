#pragma once
#include <memory>

#include "Core/CommonCore/Assertion.h"

namespace Graphics
{
   struct MaterialNode
   {
      enum class eMaterialNodeType
      {
         VALUE,
         UNARY_OP,
         BINARY_OP,
      };

      virtual eMaterialNodeType GetMaterialOperationType() = 0;
   };

   struct MaterialValueNode
      : public MaterialNode
   {
      virtual eMaterialNodeType GetMaterialOperationType() override
      {
         return MaterialNode::eMaterialNodeType::VALUE;
      }
   };

   struct MaterialUnaryOperationNode
      : public MaterialNode
   {
      virtual eMaterialNodeType GetMaterialOperationType() {
         return MaterialNode::eMaterialNodeType::UNARY_OP;
      }

      MaterialNode* InputOperation;
      MaterialValueNode* OutputValue;
   };

   struct MaterialBinaryOperationNode
      : public MaterialNode
   {
      virtual eMaterialNodeType GetMaterialOperationType() {
         return MaterialNode::eMaterialNodeType::BINARY_OP;
      }

      MaterialNode* InputOperation1;
      MaterialNode* InputOperation2;
      MaterialValueNode* OutputValue;
   };

   /********************/

   struct MaterialOperationProperty
   {
      enum class eMaterialOperationPropertyType {
         FLOAT_PROPERTY,
      };

      virtual eMaterialOperationPropertyType GetMaterialOperationPropertyType() = 0;
   };

   struct MaterialOperationFloatProperty
      : public MaterialOperationProperty
   {
      float Value;

      MaterialOperationFloatProperty(const float value) : Value(value) {}

      virtual eMaterialOperationPropertyType GetMaterialOperationPropertyType()
      {
         return MaterialOperationProperty::eMaterialOperationPropertyType::FLOAT_PROPERTY;
      }
   };

   class MaterialPropertyInOut
   {
   public:

      std::shared_ptr<MaterialOperationProperty> value;
   };

   class MaterialOperation
   {
   public:
      MaterialOperation();
      ~MaterialOperation();
   };

   class UnaryMaterialOperation
   {
   public:
      virtual MaterialPropertyInOut Process(MaterialPropertyInOut src) = 0;
   };

   class UnaryIncrementOperation
      : public UnaryMaterialOperation
   {
   public:
      virtual MaterialPropertyInOut Process(MaterialPropertyInOut src) override
      {
         MaterialPropertyInOut result;

         if (!src.value)
            assert(false);

         if (src.value->GetMaterialOperationPropertyType() == MaterialOperationProperty::eMaterialOperationPropertyType::FLOAT_PROPERTY)
         {
            auto floatProperty = std::static_pointer_cast<MaterialOperationFloatProperty>(src.value);
            result.value = std::make_shared<MaterialOperationFloatProperty>(floatProperty->Value + 1.0f);
         }
         else
         {
            assert(false);
         }

         return result;
      }
   };

   class BinaryMaterialOperation
   {
   public:
      virtual MaterialPropertyInOut Process(MaterialPropertyInOut src1, MaterialPropertyInOut src2) = 0;
   };

   class BinarySumOperation
      : public BinaryMaterialOperation
   {
   public:
      virtual MaterialPropertyInOut Process(MaterialPropertyInOut src1, MaterialPropertyInOut src2) override;
   };

}

