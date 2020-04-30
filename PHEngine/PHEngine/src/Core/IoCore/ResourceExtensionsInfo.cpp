#include "ResourceExtensionsInfo.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

using namespace EngineUtility;

namespace IO
{

   std::map<std::string, RESOURCE_TYPE> ResourceExtensionsInfo::Extensions = std::map<std::string, RESOURCE_TYPE>(
   {
      std::pair("png", RESOURCE_TYPE::TEXTURE),
      std::pair("bmp", RESOURCE_TYPE::TEXTURE),
      std::pair("jpg", RESOURCE_TYPE::TEXTURE),
      std::pair("obj", RESOURCE_TYPE::MESH),
      std::pair("dae", RESOURCE_TYPE::MESH),
   });

   RESOURCE_TYPE ResourceExtensionsInfo::GetResourceTypeByFileExtension(const std::string& filePath) 
   {
      const size_t extensionStartIndex = LastIndexOf(filePath, ".");

      if (std::string::npos == extensionStartIndex)
         return RESOURCE_TYPE::UNDEFINED_TYPE;

      std::string extension = filePath.substr(extensionStartIndex + 1);

      return GetResourceType(extension);
   }

   RESOURCE_TYPE ResourceExtensionsInfo::GetResourceType(const std::string& extensionName)
   {
      return Extensions[extensionName];
   }
}