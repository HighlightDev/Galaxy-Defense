#include "MaterialBinarySubOperationNode.h"

namespace Graphics
{

   MaterialNode::eMaterialOperationType MaterialBinarySubOperationNode::GetMaterialOperationType() const 
   {
      return MaterialNode::eMaterialOperationType::BINARY_SUB;
   }

   float MaterialBinarySubOperationNode::DoOperation(const float& left, const float& right) 
   {
      return left - right;
   }
}
