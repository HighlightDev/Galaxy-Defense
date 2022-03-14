#include "MaterialUnaryNoOperationNode.h"

namespace Graphics
{
   MaterialNode::eMaterialOperationType MaterialUnaryNoOperationNode::GetMaterialOperationType() const
   {
      return MaterialNode::eMaterialOperationType::UNARY_NO_OPERATION;
   }

   float MaterialUnaryNoOperationNode::DoOperation(const float& value)
   {
      return value;
   }
}
