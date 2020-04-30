#include "ResourceExtensionsInfo.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

using namespace EngineUtility;

namespace IO
{

   std::map<std::string, RESOURCE_TYPE> ResourceExtensionsInfo::Extensions = std::map<std::string, RESOURCE_TYPE>(
   {
      std::pair("", RESOURCE_TYPE::TEXTURE),
   });

   RESOURCE_TYPE ResourceExtensionsInfo::GetResourceTypeByFileExtension(const std::string& filePath) 
   {
      const size_t extensionStartIndex = LastIndexOf(filePath, ".");

      std::string extension = filePath.substr(extensionStartIndex);
   }

   RESOURCE_TYPE ResourceExtensionsInfo::GetResourceType(const std::string& extensionName)
   {


   }
}