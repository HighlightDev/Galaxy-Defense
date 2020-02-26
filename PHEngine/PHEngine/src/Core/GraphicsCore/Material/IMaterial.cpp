#include "IMaterial.h"

namespace Graphics
{

   IMaterial::IMaterial(const std::string& materialName, const std::string& relativeMaterialShaderPath)
      : MaterialName(materialName)
      , RelativeMaterialShaderPath(relativeMaterialShaderPath)
   {
   }

   IMaterial::~IMaterial()
   {
   }
}
