#include "DynamicMaterial.h"


namespace Graphics
{
   DynamicMaterial::DynamicMaterial(const std::string& materialName, const std::string& materialShaderName)
      : IMaterial(materialName, materialShaderName)
   {
   }

   DynamicMaterial::~DynamicMaterial()
   {
   }
}