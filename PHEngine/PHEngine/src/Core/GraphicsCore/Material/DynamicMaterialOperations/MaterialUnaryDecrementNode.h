#pragma once
#include "MaterialUnaryOperationNode.h"

namespace Graphics
{
   struct MaterialUnaryDecrementNode
       : public MaterialUnaryOperationNode
   {

      explicit MaterialUnaryDecrementNode(const MaterialNode::eMaterialPropertyType materialPropertyType);

      eMaterialOperationType GetMaterialOperationType() const override;

      std::any DoOperation(const std::any &value) override;
   };
}
