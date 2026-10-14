#include "MaterialConstantFloatValueNode.h"

namespace Graphics {
MaterialConstantFloatValueNode::MaterialConstantFloatValueNode(const float value)
    : MaterialValueNode(MaterialNode::eMaterialPropertyType::FLOAT)
    , mValue(value)
{
}

std::any MaterialConstantFloatValueNode::TraverseGraph()
{
    return mValue;
}

MaterialValueNode::eValueType MaterialConstantFloatValueNode::GetValueType() const
{
    return MaterialValueNode::eValueType::FLOAT_CONSTANT;
}
} // namespace Graphics
