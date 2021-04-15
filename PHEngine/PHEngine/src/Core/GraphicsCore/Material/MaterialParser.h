#pragma once

#include "Core/GraphicsCore/Material/IMaterial.h"

#include <list>

namespace Graphics
{
   class MaterialParser
   {
   public: 

      static IMaterial* ParseMaterialDescriptor(const std::string& relPathToMaterial);

   private:

      static IMaterial* ParseStaticMaterial(const std::list<std::string>& materialSrc, const std::string& materialName, const std::string& materialShaderPath);

      static IMaterial* ParseDynamicMaterial(const std::list<std::string>& materialSrc, const std::string& materialName, const std::string& materialShaderPath);


   };
}
