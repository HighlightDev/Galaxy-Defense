#pragma once

#include "Core/GraphicsCore/Material/IMaterial.h"

namespace Graphics
{
   class MaterialParser
   {
   public: 

      static IMaterial* ParseMaterialDescriptor(const std::string& relPathToMaterial);

      static IMaterial* ParseDynamicMaterialDescriptor(const std::string& relPathToMaterial);
   };
}
