#include "MaterialParser.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"
#include "Core/IoCore/FileFacade.h"

#include <algorithm>

namespace Graphics
{
#define GENERAL_START_NODE_NAME     "<general>"
#define GENERAL_END_NODE_NAME       "</general>"
#define PROPERTIES_START_NODE_NAME  "<properties>"
#define PROPERTIES_END_NODE_NAME    "</properties>"
#define PROPERTY_START_NODE_NAME    "<property>"
#define PROPERTY_END_NODE_NAME      "</property>"

   using iterator_t = typename std::list<std::string>::const_iterator;

   iterator_t GetItByNodeName(const std::list<std::string>& fileSource, const std::string& nodeName)
   {
      return std::find_if(fileSource.begin(), fileSource.end(), [&](const std::string& srcNode) { return EngineUtility::TrimStart(srcNode) == nodeName; });
   }

   iterator_t GetItByNodeName(const iterator_t& beginIt, const iterator_t& endIt, const std::string& nodeName)
   {
      return std::find_if(beginIt, endIt, [&](const std::string& srcNode) { return EngineUtility::TrimStart(srcNode) == nodeName; });
   }

   std::string GetPropertyNodeByName(const std::string& trimmedNodeStr, const std::string& propName)
   {
      std::string result;

      const size_t nameStartIndex = EngineUtility::IndexOf(trimmedNodeStr, "\"") + 1;
      const size_t nameEndIndex = EngineUtility::IndexOf(trimmedNodeStr, "\"", nameStartIndex);
      result = trimmedNodeStr.substr(nameStartIndex, nameEndIndex - nameStartIndex);

      return result;
   }

   std::shared_ptr<MaterialProperty> CreatePropertyByType(const std::string& propertyType)
   {
      std::shared_ptr<MaterialProperty> resultProperty;

      if ("texture" == propertyType)
      {
         resultProperty = std::make_shared<TextureMaterialProperty>();
      }
      else if ("float" == propertyType)
      {
         resultProperty = std::make_shared<FloatMaterialProperty>();
      }
      else
      {
         assert((false, "Unknown property type."));
      }

      return resultProperty;
   }

   std::shared_ptr<MaterialProperty> GetMaterialPropertyAndAdvanceIterator(iterator_t& propertiesBeginIt, const iterator_t& propertiesEndIt, std::string& outPropertyName)
   {
      auto propertyStartNode = GetItByNodeName(propertiesBeginIt, propertiesEndIt, PROPERTY_START_NODE_NAME);
      auto propertyEndNode = GetItByNodeName(propertiesBeginIt, propertiesEndIt, PROPERTY_END_NODE_NAME);

      assert(propertyStartNode != propertiesEndIt);

      std::string propertyName, propertyType;

      ++propertyStartNode;
      for (auto it = propertyStartNode; it != propertyEndNode; ++it)
      {
         const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

         if (EngineUtility::StartsWith(currentNodeStr, "name"))
         {
            propertyName = GetPropertyNodeByName(currentNodeStr, "name");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "type"))
         {
            propertyType = GetPropertyNodeByName(currentNodeStr, "type");
         }
      }

      outPropertyName = propertyName;
      propertiesBeginIt = propertyEndNode;

      return CreatePropertyByType(propertyType);
   }

   IMaterial* MaterialParser::ParseMaterialDescriptor(const std::string& relPathToMaterial)
   {
      const std::string& absolutePath = EngineUtility::ConvertFromRelativeToAbsolutePath(relPathToMaterial);
      FileFacade fileWorker(absolutePath);

      const size_t sizeOfSrc = fileWorker.GetFileSourceSize();
      assert(sizeOfSrc > 0);

      std::list<std::string> fileSource = fileWorker.GetFileSrc();

      std::string materialName;
      std::string materialShaderName;

      auto generalStartNode = GetItByNodeName(fileSource, GENERAL_START_NODE_NAME);
      auto generalEndNode = GetItByNodeName(fileSource, GENERAL_END_NODE_NAME);

      ++generalStartNode;
      for (auto it = generalStartNode; it != generalEndNode; ++it)
      {
         const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

         if (EngineUtility::StartsWith(currentNodeStr, "name"))
         {
            materialName = GetPropertyNodeByName(currentNodeStr, "name");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "shader"))
         {
            materialShaderName = GetPropertyNodeByName(currentNodeStr, "shader");
         }
      }

      IMaterial* material = new IMaterial(materialName, materialShaderName);

      auto propertiesStartNode = GetItByNodeName(fileSource, PROPERTIES_START_NODE_NAME);
      auto propertiesEndNode = GetItByNodeName(fileSource, PROPERTIES_END_NODE_NAME);

      ++propertiesStartNode;
      for (auto it = propertiesStartNode; it != propertiesEndNode; ++it)
      {
         std::string propertyName;
         std::shared_ptr<MaterialProperty> materialProperty = GetMaterialPropertyAndAdvanceIterator(it, propertiesEndNode, propertyName);
         material->PushMaterialProperty(propertyName, std::move(materialProperty));
      }

      return material;
   }

#undef GENERAL_START_NODE_NAME    
#undef GENERAL_END_NODE_NAME      
#undef PROPERTIES_START_NODE_NAME 
#undef PROPERTIES_END_NODE_NAME   
#undef PROPERTY_START_NODE_NAME   
#undef PROPERTY_END_NODE_NAME     
}