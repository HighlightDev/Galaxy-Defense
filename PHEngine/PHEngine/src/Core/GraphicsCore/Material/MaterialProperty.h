#pragma once

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

using namespace Graphics::Texture;
using namespace Graphics::OpenGL;

struct MaterialProperty
{
   enum class MaterialPropertyType
   {
      FLOAT_PROPERTY,
      TEXTURE_PROPERTY,
   };

public:

   virtual void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const = 0;

   virtual MaterialPropertyType GetMaterialPropertyType() const = 0;

};

struct FloatMaterialProperty
   : public MaterialProperty
{
   using MaterialPropertyValueType = float;

private:

   MaterialPropertyValueType m_value;

public:

   FloatMaterialProperty(MaterialPropertyValueType propertyValue)
      : MaterialProperty()
      , m_value(propertyValue)
   {
   }

   FloatMaterialProperty()
      : MaterialProperty()
      , m_value(0.0f)
   {
   }

   virtual MaterialPropertyType GetMaterialPropertyType() const override
   {
      return MaterialProperty::MaterialPropertyType::FLOAT_PROPERTY;
   }

   virtual void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override
   {
      uniform.LoadUniform(m_value);
   }

   inline void SetValue(MaterialPropertyValueType value) {
      m_value = value;
   }

   inline float GetValue() const {
      return m_value;
   }

};

struct TextureMaterialProperty 
   : public MaterialProperty
{
   using MaterialPropertyValueType = std::shared_ptr<ITexture>;

private :

   MaterialPropertyValueType m_value;

public:

   TextureMaterialProperty(MaterialPropertyValueType propertyValue)
      : MaterialProperty()
      , m_value(propertyValue)
   {
   }

   TextureMaterialProperty()
      : MaterialProperty()
   {
   }

   virtual MaterialPropertyType GetMaterialPropertyType() const override
   {
      return MaterialProperty::MaterialPropertyType::TEXTURE_PROPERTY;
   }

   virtual void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const override
   {
      int32_t slot = 10 + propertyIndex;
      if (m_value)
      {
         m_value->BindTexture(slot);
         uniform.LoadUniform(slot);
      }
   }

   inline void SetValue(MaterialPropertyValueType value) {
      m_value = value;
   }

   inline void SetValue(ITexture* value) {
      m_value = std::shared_ptr<ITexture>(value);
   }

   inline std::shared_ptr<ITexture> GetValue() const {
      return m_value;
   }
};
