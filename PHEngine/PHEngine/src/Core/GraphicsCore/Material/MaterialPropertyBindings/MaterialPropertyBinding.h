#pragma once

namespace Graphics {
   enum class eMaterialBindingType
   {
      FLOAT
   };

   struct MaterialPropertyBinding
   {
      std::string BindingName;
      std::string GameObjectName;
      std::string GameObjectPropertyName;

      MaterialPropertyBinding(const std::string& bindingName)
         : BindingName(bindingName)
         , GameObjectName("")
         , GameObjectPropertyName("")
      {
      }

      virtual eMaterialBindingType GetMaterialBindingType() const = 0;
   };
}