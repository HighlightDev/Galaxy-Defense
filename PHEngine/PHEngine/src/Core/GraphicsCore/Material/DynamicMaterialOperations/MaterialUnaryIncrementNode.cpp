#include "MaterialUnaryIncrementNode.h"

namespace Graphics
{
   MaterialNode::eMaterialOperationType MaterialUnaryIncrementNode::GetMaterialOperationType() const
   {
      return MaterialNode::eMaterialOperationType::UNARY_INCREMENT;
   }

   float MaterialUnaryIncrementNode::DoOperation(const float& value)
   {
      return value + 1.0f;
   }
}
