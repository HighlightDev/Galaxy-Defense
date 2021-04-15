#pragma once

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

using namespace Graphics::Texture;
using namespace Graphics::OpenGL;

namespace Graphics {

   struct MaterialProperty
   {
      enum class MaterialPropertyType
      {
         FLOAT_PROPERTY,
         TEXTURE_PROPERTY,
         DEFERRED_TEXTURE_PROPERTY,
         DYNAMIC_FLOAT_PROPERTY,
         FLOAT_BINDING_PROPERTY,
      };

   protected:

      std::string mPropertyName;

   public:

      MaterialProperty(const std::string& propertyName)
         : mPropertyName(propertyName)
      {
      }

      virtual void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const = 0;

      virtual MaterialPropertyType GetPropertyType() const = 0;

      std::string GetPropertyName() const
      {
         return mPropertyName;
      }

   };
}
