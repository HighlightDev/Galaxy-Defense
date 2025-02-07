#include "MaterialValuePropertyNode.h"

#include "Core/GraphicsCore/Material/MaterialProperties/FloatBindingMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/FloatMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/Vec2BindingMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/Vec2MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/Vec3BindingMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/Vec3MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/iVec2BindingMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/iVec2MaterialProperty.h"

namespace Graphics {
MaterialValuePropertyNode::~MaterialValuePropertyNode()
{
}

MaterialValuePropertyNode::MaterialValuePropertyNode(
    std::shared_ptr<MaterialProperty> valueProperty, const MaterialNode::eMaterialPropertyType materialNodeType)
    : MaterialValueNode(materialNodeType)
    , mValueProperty(valueProperty)
{
}

MaterialValueNode::eValueType MaterialValuePropertyNode::GetValueType() const
{
    return MaterialValueNode::eValueType::PROPERTY;
}

std::shared_ptr<MaterialProperty> MaterialValuePropertyNode::GetValueProperty() const
{
    return mValueProperty;
}

std::any MaterialValuePropertyNode::TraverseGraph()
{
    if (mValueProperty->GetPropertyType() == MaterialProperty::eMaterialPropertyType::FLOAT_PROPERTY) {
        return std::static_pointer_cast<FloatMaterialProperty>(mValueProperty)->GetValue();
    } else if (mValueProperty->GetPropertyType() == MaterialProperty::eMaterialPropertyType::FLOAT_BINDING_PROPERTY) {
        return std::static_pointer_cast<FloatBindingMaterialProperty>(mValueProperty)->GetValue();
    } else if (mValueProperty->GetPropertyType() == MaterialProperty::eMaterialPropertyType::IVEC2_PROPERTY) {
        return std::static_pointer_cast<iVec2MaterialProperty>(mValueProperty)->GetValue();
    } else if (mValueProperty->GetPropertyType() == MaterialProperty::eMaterialPropertyType::IVEC2_BINDING_PROPERTY) {
        return std::static_pointer_cast<iVec2BindingMaterialProperty>(mValueProperty)->GetValue();
    } else if (mValueProperty->GetPropertyType() == MaterialProperty::eMaterialPropertyType::VEC2_PROPERTY) {
        return std::static_pointer_cast<Vec2MaterialProperty>(mValueProperty)->GetValue();
    } else if (mValueProperty->GetPropertyType() == MaterialProperty::eMaterialPropertyType::VEC3_PROPERTY) {
        return std::static_pointer_cast<Vec3MaterialProperty>(mValueProperty)->GetValue();
    } else if (mValueProperty->GetPropertyType() == MaterialProperty::eMaterialPropertyType::VEC2_BINDING_PROPERTY) {
        return std::static_pointer_cast<Vec2BindingMaterialProperty>(mValueProperty)->GetValue();
    } else if (mValueProperty->GetPropertyType() == MaterialProperty::eMaterialPropertyType::VEC3_BINDING_PROPERTY) {
        return std::static_pointer_cast<Vec3BindingMaterialProperty>(mValueProperty)->GetValue();
    } else {
        assert(false); // Inaccessible code
    }

    return -1;
}
} // namespace Graphics