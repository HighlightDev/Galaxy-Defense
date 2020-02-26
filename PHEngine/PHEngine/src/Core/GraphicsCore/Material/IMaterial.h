#pragma once

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Material/MaterialProperty.h"

#include <string>
#include <set>
#include <tuple>
#include <unordered_map>

using namespace Graphics::Texture;

namespace Graphics
{

   class IMaterial
   {
   protected:

      std::unordered_map<std::string, std::shared_ptr<MaterialProperty>> mProperties;

   public :

      const std::string MaterialName;
      const std::string RelativeMaterialShaderPath;

   public:

      IMaterial(const std::string& materialName, const std::string& relativeMaterialShaderPath);

      virtual ~IMaterial();

      const std::unordered_map<std::string, std::shared_ptr<MaterialProperty>>& GetProperties() const
      {
         return mProperties;
      }
   };

}

