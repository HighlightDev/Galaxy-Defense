#pragma once

#include "Core/GraphicsCore/Material/MaterialProperty.h"

#include <string>
#include <unordered_map>

using namespace Graphics::Texture;

namespace Graphics
{

   class MaterialProxy
   {
      static size_t MaterialProxyIdCounter;

   public:

      const std::string MaterialName;
      const std::string MaterialShaderName;
      const std::string MaterialShaderRelativePath;

   protected:

      size_t mMaterialProxyId;

      std::unordered_map<std::string, std::shared_ptr<MaterialProperty>> mProperties;

   public:

      MaterialProxy(const class IMaterial* material);

      ~MaterialProxy();

      const std::unordered_map<std::string, std::shared_ptr<MaterialProperty>>& GetProperties() const;

   private:

      void UpdateProperty(const std::string& propertyName, std::shared_ptr<MaterialProperty> property);

      void UpdateProperties(const std::unordered_map<std::string, std::shared_ptr<MaterialProperty>>& updatedProperties);
   };

}

