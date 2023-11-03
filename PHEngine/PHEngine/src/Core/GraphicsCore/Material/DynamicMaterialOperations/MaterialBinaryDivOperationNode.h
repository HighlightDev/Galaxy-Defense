#pragma once
#include "MaterialBinaryOperationNode.h"

namespace Graphics
{
   struct MaterialBinaryDivOperationNode
      : public MaterialBinaryOperationNode
   {
      MaterialBinaryDivOperationNode(const MaterialNode::eMaterialPropertyType materialPropertyType);

      eMaterialOperationType GetMaterialOperationType() const override;

      std::any DoOperation(const std::any& left, const std::any& right) override;
   };
}

