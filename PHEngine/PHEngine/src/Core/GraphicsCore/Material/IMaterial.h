#pragma once

#include "Core/GraphicsCore/Material/MaterialProperty.h"

#include <string>
#include <set>
#include <tuple>
#include <unordered_map>

using namespace Graphics::Texture;

namespace Graphics
{

   struct MaterialPropertySetter
   {
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, ITexture* texture);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, std::shared_ptr<ITexture> texture);
      static void SetMaterialPropertyValue(class IMaterial* materialInstance, const std::string& propertyName, float value);
   };

   class IMaterial
   {
   protected:

      using genericMaterialPropertyPair_t = std::pair<std::string, std::shared_ptr<MaterialProperty>>;

      std::unordered_map<std::string, std::shared_ptr<MaterialProperty>> mProperties;

   public :

      const std::string MaterialName;
      const std::string RelativeMaterialShaderPath;

   public:

      IMaterial(const std::string& materialName, const std::string& materialShaderName);

      virtual ~IMaterial();

      std::shared_ptr<MaterialProperty> GetMaterialPropertyByName(const std::string& propertyName) const;

      void PushMaterialProperty(const std::string& propertyName, std::shared_ptr<MaterialProperty>&& propertyValue);

      const std::unordered_map<std::string, std::shared_ptr<MaterialProperty>>& GetProperties() const;
   };

}

