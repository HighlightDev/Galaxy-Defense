#pragma once
#include "MaterialBinaryOperationNode.h"

namespace Graphics
{
   struct MaterialBinaryMulOperationNode
      : public MaterialBinaryOperationNode
   {
      virtual eMaterialOperationType GetMaterialOperationType() const override;

      virtual float DoOperation(const float& left, const float& right) override;
   };
}

