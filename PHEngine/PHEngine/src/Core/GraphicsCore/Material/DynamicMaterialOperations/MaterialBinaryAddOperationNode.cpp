#include "MaterialBinaryAddOperationNode.h"


namespace Graphics
{
   MaterialNode::eMaterialOperationType MaterialBinaryAddOperationNode::GetMaterialOperationType() const
   {
      return MaterialNode::eMaterialOperationType::BINARY_ADD;
   }

   float MaterialBinaryAddOperationNode::DoOperation(const float& left, const float& right)
   {
      return left + right;
   }
}
