#pragma once
#include "MaterialBinaryOperationNode.h"

namespace Graphics
{
   struct MaterialBinaryDivOperationNode
      : public MaterialBinaryOperationNode
   {
      virtual eMaterialOperationType GetMaterialOperationType() const override;

      virtual float DoOperation(const float& left, const float& right) override;
   };
}

