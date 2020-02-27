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

      using genericMaterialPropertyPair_t = std::pair<std::string, std::shared_ptr<MaterialProperty>>;

      std::unordered_map<std::string, std::shared_ptr<MaterialProperty>> mProperties;

   public :

      const std::string MaterialName;
      const std::string RelativeMaterialShaderPath;

   public:

      IMaterial(const std::string& materialName, const std::string& relativeMaterialShaderPath);

      virtual ~IMaterial();

      void PushMaterialProperty(const std::string& propertyName, std::shared_ptr<MaterialProperty>&& propertyValue) {
         mProperties.emplace(std::make_pair(propertyName, std::forward<std::shared_ptr<MaterialProperty>>(propertyValue)));
      }

      const std::unordered_map<std::string, std::shared_ptr<MaterialProperty>>& GetProperties() const
      {
         return mProperties;
      }
   };

}

