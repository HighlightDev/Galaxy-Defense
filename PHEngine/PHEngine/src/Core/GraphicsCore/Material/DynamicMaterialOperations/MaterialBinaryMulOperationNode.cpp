#include "MaterialBinaryMulOperationNode.h"

namespace Graphics
{
   MaterialNode::eMaterialOperationType MaterialBinaryMulOperationNode::GetMaterialOperationType() const 
   {
      return MaterialNode::eMaterialOperationType::BINARY_MUL;
   }

   float MaterialBinaryMulOperationNode::DoOperation(const float& left, const float& right) 
   {
      return left * right;
   }
}
