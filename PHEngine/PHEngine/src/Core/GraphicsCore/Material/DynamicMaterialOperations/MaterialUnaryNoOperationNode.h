#pragma once
#include "MaterialUnaryOperationNode.h"

namespace Graphics
{
   struct MaterialUnaryNoOperationNode
      : public MaterialUnaryOperationNode {

      virtual eMaterialOperationType GetMaterialOperationType() const;

      virtual float DoOperation(const float& value) override;
   };
}