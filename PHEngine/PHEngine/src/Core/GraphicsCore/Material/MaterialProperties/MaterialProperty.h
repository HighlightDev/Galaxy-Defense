#pragma once

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"

using namespace Graphics::Texture;
using namespace Graphics::OpenGL;

namespace Graphics {

   struct MaterialProperty
   {
      enum class eMaterialPropertyType
      {
         FLOAT_PROPERTY,
         TEXTURE_PROPERTY,
         DEFERRED_TEXTURE_PROPERTY,
         FLOAT_BINDING_PROPERTY,
         IVEC2_BINDING_PROPERTY,
         VEC2_BINDING_PROPERTY,
         VEC3_BINDING_PROPERTY,
         VEC2_PROPERTY,
         IVEC2_PROPERTY,
         VEC3_PROPERTY
      };

   protected:

      std::string mPropertyName;

   public:

      MaterialProperty(const std::string& propertyName)
         : mPropertyName(propertyName)
      {
      }

      virtual void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const = 0;

      virtual eMaterialPropertyType GetPropertyType() const = 0;

      std::string GetPropertyName() const
      {
         return mPropertyName;
      }

   };
}
