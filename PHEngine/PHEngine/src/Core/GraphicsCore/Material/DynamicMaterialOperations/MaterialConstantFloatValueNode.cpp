#include "MaterialConstantFloatValueNode.h"

namespace Graphics
{
   MaterialConstantFloatValueNode::MaterialConstantFloatValueNode(const float value)
      : mValue(value)
   {
   }

   float MaterialConstantFloatValueNode::TraverseGraph()
   {
      return mValue;
   }

   MaterialValueNode::eValueType MaterialConstantFloatValueNode::GetValueType() const {
      return MaterialValueNode::eValueType::FLOAT_CONSTANT;
   }
}

