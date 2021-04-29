#include "MaterialValuePropertyNode.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/FloatMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/FloatBindingMaterialProperty.h"

namespace Graphics
{

   MaterialValuePropertyNode::MaterialValuePropertyNode()
   {
   }

   MaterialValuePropertyNode::~MaterialValuePropertyNode()
   {
   }

   MaterialValuePropertyNode::MaterialValuePropertyNode(std::shared_ptr<MaterialProperty> valueProperty)
      : mValueProperty(valueProperty)
   {
   }

   MaterialValueNode::eValueType MaterialValuePropertyNode::GetValueType() const {
      return MaterialValueNode::eValueType::PROPERTY;
   }

   std::shared_ptr<MaterialProperty> MaterialValuePropertyNode::GetValueProperty() const
   {
      return mValueProperty;
   }

   float MaterialValuePropertyNode::TraverseGraph()
   {
      if (mValueProperty->GetPropertyType() == MaterialProperty::eMaterialPropertyType::FLOAT_PROPERTY)
      {
         return std::static_pointer_cast<FloatMaterialProperty>(mValueProperty)->GetValue();
      }
      else if (mValueProperty->GetPropertyType() == MaterialProperty::eMaterialPropertyType::FLOAT_BINDING_PROPERTY)
      {
         return std::static_pointer_cast<FloatBindingMaterialProperty>(mValueProperty)->GetValue();
      }

      // Inaccessible code
      else assert(false);
      return 0.0f;
   }
}