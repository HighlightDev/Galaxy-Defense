#pragma once
#include "MaterialUnaryOperationNode.h"

namespace Graphics
{
   struct MaterialUnaryIncrementNode
      : public MaterialUnaryOperationNode {

      virtual eMaterialOperationType GetMaterialOperationType() const;

      virtual float DoOperation(const float& value) override;
   };
}
