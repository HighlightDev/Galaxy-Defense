#include "DynamicMaterial.h"


namespace Graphics
{
   DynamicMaterial::DynamicMaterial(const std::string& materialName, const std::string& relativeMaterialShaderPath)
      : IMaterial(materialName, relativeMaterialShaderPath)
   {
   }

   DynamicMaterial::~DynamicMaterial()
   {
   }
}