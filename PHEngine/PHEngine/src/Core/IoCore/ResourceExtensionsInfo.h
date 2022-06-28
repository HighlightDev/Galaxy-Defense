#pragma once

#include <string>
#include <map>

namespace IO
{
   enum class eResourceType 
   {
      UNDEFINED_TYPE,
      TEXTURE,
      MESH,
      AUDIO
   };

   struct ResourceExtensionsInfo
   {
      static eResourceType GetResourceTypeByFileExtension(const std::string& filePath);

   private:

      static std::map<std::string, eResourceType> Extensions;

      static eResourceType GetResourceType(const std::string& extensionName);
   };
}