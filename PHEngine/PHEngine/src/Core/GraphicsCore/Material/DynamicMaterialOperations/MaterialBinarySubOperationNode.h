#pragma once
#include "MaterialBinaryOperationNode.h"

namespace Graphics
{

   struct MaterialBinarySubOperationNode
      : public MaterialBinaryOperationNode
   {
      virtual eMaterialOperationType GetMaterialOperationType() const override;

      virtual float DoOperation(const float& left, const float& right) override;
   };

}
