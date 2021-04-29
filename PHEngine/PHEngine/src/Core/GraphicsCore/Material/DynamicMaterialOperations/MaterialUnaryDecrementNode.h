#pragma once
#include "MaterialUnaryOperationNode.h"

namespace Graphics
{
   struct MaterialUnaryDecrementNode
      : public MaterialUnaryOperationNode {

      virtual eMaterialOperationType GetMaterialOperationType() const override;

      virtual float DoOperation(const float& value) override;
   };
}
