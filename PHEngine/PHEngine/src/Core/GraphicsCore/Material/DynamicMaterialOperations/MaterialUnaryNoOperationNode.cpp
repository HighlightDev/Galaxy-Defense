#include "MaterialUnaryNoOperationNode.h"

namespace Graphics
{
   MaterialUnaryNoOperationNode::MaterialUnaryNoOperationNode(const MaterialNode::eMaterialPropertyType materialPropertyType)
       : MaterialUnaryOperationNode(materialPropertyType)
   {
   }

   MaterialNode::eMaterialOperationType MaterialUnaryNoOperationNode::GetMaterialOperationType() const
   {
      return MaterialNode::eMaterialOperationType::UNARY_NO_OPERATION;
   }

   std::any MaterialUnaryNoOperationNode::DoOperation(const std::any& value)
   {
      return value;
   }
}
