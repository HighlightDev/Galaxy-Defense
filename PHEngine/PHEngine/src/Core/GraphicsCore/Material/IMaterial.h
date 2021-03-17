#pragma once

#include "MaterialProperty.h"
#include "MaterialProxy.h"

#include <string>
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

      size_t MaterialProxyId;

      const std::string MaterialName;
      const std::string MaterialShaderName;
      const std::string MaterialShaderRelativePath;

   public:

      IMaterial(const std::string& materialName, const std::string& materialShaderName);

      virtual ~IMaterial();

      std::shared_ptr<MaterialProperty> GetMaterialPropertyByName(const std::string& propertyName) const;

      void PushMaterialProperty(const std::string& propertyName, std::shared_ptr<MaterialProperty>&& propertyValue);

      const std::unordered_map<std::string, std::shared_ptr<MaterialProperty>>& GetProperties() const;

      virtual std::shared_ptr<MaterialProxy> CreateMaterialProxy() const;
   };

}

