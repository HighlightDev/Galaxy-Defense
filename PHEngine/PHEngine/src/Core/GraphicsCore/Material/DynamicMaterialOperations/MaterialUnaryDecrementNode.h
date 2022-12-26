#pragma once
#include "MaterialUnaryOperationNode.h"

namespace Graphics
{
   struct MaterialUnaryDecrementNode
      : public MaterialUnaryOperationNode {

      eMaterialOperationType GetMaterialOperationType() const override;

      float DoOperation(const float& value) override;
   };
}
