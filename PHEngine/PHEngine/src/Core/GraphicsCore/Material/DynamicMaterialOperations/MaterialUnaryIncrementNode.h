#pragma once
#include "MaterialUnaryOperationNode.h"

namespace Graphics
{
   struct MaterialUnaryIncrementNode
      : public MaterialUnaryOperationNode {

      virtual eMaterialOperationType GetMaterialOperationType() const;

      float DoOperation(const float& value) override;
   };
}
