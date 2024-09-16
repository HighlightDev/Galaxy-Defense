#pragma once

#include "BindingMaterialProperty.h"
#include "Core/GameCore/EngineObjectPropertyBindings/Vec2PropertyBinding.h"

using namespace EngineCore;

namespace Graphics {

   struct Vec2BindingMaterialProperty
      : public BindingMaterialProperty

   {
      using MaterialPropertyValueType = std::shared_ptr<Vec2PropertyBinding>;

   public:

      Vec2BindingMaterialProperty(MaterialPropertyValueType propertyValue, const std::string& propertyName)
         : BindingMaterialProperty(propertyValue, propertyName)
      {
      }

      Vec2BindingMaterialProperty(const std::string& propertyName)
         : BindingMaterialProperty(propertyName)
      {
      }

      eMaterialPropertyType GetPropertyType() const override
      {
         return MaterialProperty::eMaterialPropertyType::VEC2_BINDING_PROPERTY;
      }

      void SetValueToUniformArray(const UniformArray& uniformArray) const override
      {
      }

      void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override { assert(false); } // this code should not be called, use proxy on render thread part 

      void SetValue(MaterialPropertyValueType value) {
         mPropertyBinding = value;
      }

      glm::vec2 GetValue() const {
         return std::static_pointer_cast<Vec2PropertyBinding>(mPropertyBinding)->GetValue();
      }

   };
}
