#include "MaterialOperation.h"

#include "Core/GraphicsCore/Material/MaterialProperties/FloatMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/FloatBindingMaterialProperty.h"

namespace Graphics
{
   //todo: Refactor this

   bool IsPropertyBindingType(std::shared_ptr<MaterialProperty> property, MaterialProperty::eMaterialPropertyType& outPropertyType)
   {
      outPropertyType = property->GetPropertyType();
      switch (outPropertyType)
      {
         case MaterialProperty::eMaterialPropertyType::FLOAT_BINDING_PROPERTY:
            return true;
         default:
            return false;
      }
   }

   float MaterialStartNode::getIteratedValue(std::shared_ptr<MaterialNode> node)
   {
      float result = 0.0f;

      switch (node->GetMaterialNodeType())
      {
         case MaterialNode::eMaterialNodeType::START:
         {
            auto startNode = MaterialNode::CastTo<MaterialStartNode>(node);
            result = getIteratedValue(startNode->InputOperation);
            break;
         }
         case MaterialNode::eMaterialNodeType::UNARY_OP:
         {
            auto unaryNode = MaterialNode::CastTo<MaterialUnaryOperationNode>(node);
            result = getIteratedValue(unaryNode->InputOperation);
            result = unaryNode->DoOperation(result);
            break;
         }
         case MaterialNode::eMaterialNodeType::BINARY_OP:
         {
            auto binaryNode = MaterialNode::CastTo<MaterialBinaryOperationNode>(node);
            result = getIteratedValue(binaryNode->InputOperation1);
            result = binaryNode->DoOperation(result, getIteratedValue(binaryNode->InputOperation2));
            break;
         }
         case MaterialNode::eMaterialNodeType::VALUE:
         {
            auto valueNode = MaterialNode::CastTo<MaterialValueNode>(node);
            if (valueNode->GetValueType() == MaterialValueNode::eValueType::FLOAT_CONSTANT) {

               auto constantFloatNode = MaterialNode::CastTo<MaterialConstantFloatValueNode>(node);
               result = constantFloatNode->Value;
            }
            else if (valueNode->GetValueType() == MaterialValueNode::eValueType::PROPERTY)
            {
               const std::shared_ptr<MaterialPropertyValueNode>& propertyNode = MaterialNode::CastTo<MaterialPropertyValueNode>(node);
               if (propertyNode->Value->GetPropertyType() == MaterialProperty::eMaterialPropertyType::FLOAT_PROPERTY)
               {
                  const std::shared_ptr<FloatMaterialProperty>& floatPropertyValue = std::static_pointer_cast<FloatMaterialProperty>(propertyNode->Value);
                  result = floatPropertyValue->GetValue();
               }
               else
               {
                  MaterialProperty::eMaterialPropertyType outPropertyType;
                  if (IsPropertyBindingType(propertyNode->Value, outPropertyType))
                  {
                     if (MaterialProperty::eMaterialPropertyType::FLOAT_BINDING_PROPERTY == outPropertyType)
                     {
                        auto floatBinding = std::static_pointer_cast<FloatBindingMaterialProperty>(propertyNode->Value);
                        result = floatBinding->GetValue();
                     }
                     else {
                        assert(false);
                     }
                  }
                  else {
                     assert(false);
                  }
               }
            }
            else { assert(false); }
            break;
         }
         default:
         {
            assert(false);
            break;
         }
      }

      return result;
   }

   float MaterialStartNode::GetValue()
   {
      return getIteratedValue(InputOperation);
   }
}