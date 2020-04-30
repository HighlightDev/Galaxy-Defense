#pragma once

#include <string>
#include <map>

namespace IO
{
   enum class RESOURCE_TYPE 
   {
      TEXTURE,
      MESH,
      MESH_ANIMATION,
      // SOUND
   };

   struct ResourceExtensionsInfo
   {
      static std::map<std::string, RESOURCE_TYPE> Extensions;

      static RESOURCE_TYPE GetResourceTypeByFileExtension(const std::string& filePath);

   private:

      static RESOURCE_TYPE GetResourceType(const std::string& extensionName);
   };
}