#pragma once
#include "MaterialBinaryOperationNode.h"

namespace Graphics
{
   struct MaterialBinaryAddOperationNode
      : public MaterialBinaryOperationNode
   {
      eMaterialOperationType GetMaterialOperationType() const override;

      float DoOperation(const float& left, const float& right) override;
   };
}

