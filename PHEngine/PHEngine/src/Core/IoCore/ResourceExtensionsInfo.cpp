#include "ResourceExtensionsInfo.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

using namespace EngineUtility;

namespace IO
{

   std::map<std::string, eResourceType> ResourceExtensionsInfo::Extensions = std::map<std::string, eResourceType>(
   {
      std::pair("png", eResourceType::TEXTURE),
      std::pair("bmp", eResourceType::TEXTURE),
      std::pair("jpg", eResourceType::TEXTURE),
      std::pair("obj", eResourceType::MESH),
      std::pair("dae", eResourceType::MESH),
      std::pair("fbx", eResourceType::MESH),
      std::pair("wav", eResourceType::AUDIO)
   });

   eResourceType ResourceExtensionsInfo::GetResourceTypeByFileExtension(const std::string& filePath) 
   {
      const size_t extensionStartIndex = LastIndexOf(filePath, ".");

      if (std::string::npos == extensionStartIndex)
         return eResourceType::UNDEFINED_TYPE;

      std::string extension = filePath.substr(extensionStartIndex + 1);

      extension = EngineUtility::ToLower(extension);

      return GetResourceType(extension);
   }

   eResourceType ResourceExtensionsInfo::GetResourceType(const std::string& extensionName)
   {
      return Extensions[extensionName];
   }
}