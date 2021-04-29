#include "MaterialUnaryDecrementNode.h"

namespace Graphics
{
   MaterialNode::eMaterialOperationType MaterialUnaryDecrementNode::GetMaterialOperationType() const
   {
      return MaterialNode::eMaterialOperationType::UNARY_DECREMENT;
   }

   float MaterialUnaryDecrementNode::DoOperation(const float& value)
   {
      return value - 1.0f;
   }
}
