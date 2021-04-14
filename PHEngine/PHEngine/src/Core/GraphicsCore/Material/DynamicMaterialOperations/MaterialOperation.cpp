#include "MaterialOperation.h"

#include "Core/GraphicsCore/Material/FloatMaterialProperty.h"

namespace Graphics
{
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
            result = unaryNode->doOperation(result);
            break;
         }
         case MaterialNode::eMaterialNodeType::BINARY_OP:
         {
            auto binaryNode = MaterialNode::CastTo<MaterialBinaryOperationNode>(node);
            result = getIteratedValue(binaryNode->InputOperation1);
            result = binaryNode->doOperation(result, getIteratedValue(binaryNode->InputOperation2));
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
               if (propertyNode->Value->GetPropertyType() == MaterialProperty::MaterialPropertyType::FLOAT_PROPERTY)
               {
                  const std::shared_ptr<FloatMaterialProperty>& floatPropertyValue = std::static_pointer_cast<FloatMaterialProperty>(propertyNode->Value);
                  result = floatPropertyValue->GetValue();
               }
               else { assert(false); }
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