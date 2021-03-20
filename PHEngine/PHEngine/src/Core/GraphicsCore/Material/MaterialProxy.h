#pragma once

#include "Core/GraphicsCore/Material/MaterialProperty.h"
#include "Core/GraphicsCore/SceneProxy/SceneProxyBase.h"

#include <string>
#include <unordered_map>

using namespace Graphics::Texture;

namespace Graphics
{

   class MaterialProxy
      : public SceneProxyBase
   {
   public:

      const std::string MaterialName;
      const std::string MaterialShaderName;
      const std::string MaterialShaderRelativePath;

   protected:

      std::unordered_map<std::string, std::shared_ptr<MaterialProperty>> mProperties;

   public:

      MaterialProxy(const class IMaterial* material);

      ~MaterialProxy();

      const std::unordered_map<std::string, std::shared_ptr<MaterialProperty>>& GetProperties() const;

      std::vector<std::string> GetUniformNames() const;

   private:

      void UpdateProperty(const std::string& propertyName, std::shared_ptr<MaterialProperty> property);

      void UpdateProperties(const std::unordered_map<std::string, std::shared_ptr<MaterialProperty>>& updatedProperties);
   };

}

