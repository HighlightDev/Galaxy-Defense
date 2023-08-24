#pragma once

#include "MaterialProperties/MaterialProperty.h"
#include "MaterialProxy.h"

#include <string>
#include <tuple>
#include <vector>

using namespace Graphics::Texture;

namespace Graphics
{
   class IMaterial
   {
   public:
      enum class eMaterialType
      {
         STATIC,
         DYNAMIC
      };

   protected:
      std::vector<std::shared_ptr<MaterialProperty>> mProperties;

      std::weak_ptr<MaterialProxy> mMaterialProxyWp;

   public:
      int32_t MaterialProxyId;

      const std::string MaterialName;
      const std::string MaterialShaderName;
      const std::string MaterialShaderRelativePath;

   public:
      IMaterial(const std::string &materialName, const std::string &materialShaderName);

      virtual ~IMaterial();

      virtual void CleanUp();

      virtual eMaterialType GetMaterialType() const;

      std::weak_ptr<MaterialProxy> GetMaterialProxyWp() const;

      std::shared_ptr<MaterialProperty> GetMaterialPropertyByName(const std::string &propertyName) const;

      void PushMaterialProperty(std::shared_ptr<MaterialProperty> propertyValue);

      void SetMaterialProxyWp(const std::shared_ptr<MaterialProxy> &materialProxy);

      const std::vector<std::shared_ptr<MaterialProperty>> &GetProperties() const;

      virtual std::shared_ptr<MaterialProxy> CreateMaterialProxy() const;

      virtual void SetIsEnabled(const bool bIsEnabled);

      virtual bool IsEnabled() const;
   };

}
