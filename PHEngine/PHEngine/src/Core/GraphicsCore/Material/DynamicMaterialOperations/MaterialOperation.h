#pragma once

namespace Graphics
{

   struct MaterialOperationProperty
   {
      enum class eMaterialOperationPropertyType {
         FLOAT_PROPERTY,
      };

      virtual eMaterialOperationPropertyType GetMaterialOperationPropertyType() = 0;
   };

   struct MaterialOperationFloatProperty : public MaterialOperationProperty
   {
      float Value;

      virtual eMaterialOperationPropertyType GetMaterialOperationPropertyType()
      {
         return MaterialOperationProperty::eMaterialOperationPropertyType::FLOAT_PROPERTY;
      }
   };

   class MaterialPropertyInOut
   {
   public:

      std::shared_ptr<MaterialOperationProperty> materialProperty;
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
      virtual MaterialPropertyInOut* Process(MaterialPropertyInOut* src) = 0;
   };

   class BinaryMaterialOperation
   {
   public:
      virtual MaterialPropertyInOut Process(MaterialPropertyInOut* src1, MaterialPropertyInOut* src2) = 0;
   };

}

