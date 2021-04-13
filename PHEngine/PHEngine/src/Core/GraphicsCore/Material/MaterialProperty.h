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
      DEFERRED_TEXTURE_PROPERTY,
      DYNAMIC_FLOAT_PROPERTY
   };

public:

   virtual void SetValueToUniform(Uniform uniform, const int32_t propertyIndex) const = 0;

   virtual MaterialPropertyType GetMaterialPropertyType() const = 0;

};
