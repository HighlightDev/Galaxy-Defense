#pragma once

#include <string>
#include <map>

namespace IO
{
   enum class RESOURCE_TYPE 
   {
      UNDEFINED_TYPE,
      TEXTURE,
      MESH,
      // SOUND
   };

   struct ResourceExtensionsInfo
   {
      static RESOURCE_TYPE GetResourceTypeByFileExtension(const std::string& filePath);

   private:

      static std::map<std::string, RESOURCE_TYPE> Extensions;

      static RESOURCE_TYPE GetResourceType(const std::string& extensionName);
   };
}