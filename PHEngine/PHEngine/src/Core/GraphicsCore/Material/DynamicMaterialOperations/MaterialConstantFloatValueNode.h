#pragma once
#include "MaterialValueNode.h"

namespace Graphics
{
   struct MaterialConstantFloatValueNode
      : public MaterialValueNode
   {
   private:

      float mValue;

   public:

      explicit MaterialConstantFloatValueNode(const float value);

      virtual eValueType GetValueType() const override;

      virtual float TraverseGraph() override;
   };
}

