#include "MaterialOperation.h"

namespace Graphics
{
   float MaterialStartNode::getIteratedValue(std::shared_ptr<MaterialNode> node)
   {
      float result = 0.0f;
      if (node->GetMaterialNodeType() == MaterialNode::eMaterialNodeType::START)
      {
         auto startNode = std::static_pointer_cast<MaterialStartNode>(node);
         result = getIteratedValue(startNode->InputOperation);
      }
      else if (node->GetMaterialNodeType() == MaterialNode::eMaterialNodeType::UNARY_OP)
      {
         auto unaryNode = std::static_pointer_cast<MaterialUnaryOperationNode>(node);
         result = getIteratedValue(unaryNode->InputOperation);
         result = unaryNode->doOperation(result);
      }
      else if (node->GetMaterialNodeType() == MaterialNode::eMaterialNodeType::BINARY_OP)
      {
         auto binaryNode = std::static_pointer_cast<MaterialBinaryOperationNode>(node);
         result = getIteratedValue(binaryNode->InputOperation1);
         result = binaryNode->doOperation(result, getIteratedValue(binaryNode->InputOperation2));
      }
      else if (node->GetMaterialNodeType() == MaterialNode::eMaterialNodeType::VALUE)
      {
         auto valueNode = std::static_pointer_cast<MaterialValueNode>(node);
         result = valueNode->Value;
      }
      else {
         assert(false);
      }

      return result;
   }

   float MaterialStartNode::GetValue()
   {
      return getIteratedValue(InputOperation);
   }
}