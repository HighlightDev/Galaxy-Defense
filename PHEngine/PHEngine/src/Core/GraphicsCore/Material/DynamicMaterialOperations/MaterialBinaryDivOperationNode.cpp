#include "MaterialBinaryDivOperationNode.h"

namespace Graphics
{
   MaterialNode::eMaterialOperationType MaterialBinaryDivOperationNode::GetMaterialOperationType() const
   {
      return MaterialNode::eMaterialOperationType::BINARY_DIV;
   }

   float MaterialBinaryDivOperationNode::DoOperation(const float& left, const float& right)
   {
      assert(right != 0.0f);
      return left / right;
   }
}