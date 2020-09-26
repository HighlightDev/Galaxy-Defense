#include "MaterialParser.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/IoCore/FileFacade.h"
#include "Core/CommonCore/XMLParserHelper.h"

using namespace Common;

namespace Graphics
{
#define GENERAL_START_NODE_NAME     "<general>"
#define GENERAL_END_NODE_NAME       "</general>"
#define PROPERTIES_START_NODE_NAME  "<properties>"
#define PROPERTIES_END_NODE_NAME    "</properties>"
#define PROPERTY_START_NODE_NAME    "<property>"
#define PROPERTY_END_NODE_NAME      "</property>"


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

   std::shared_ptr<MaterialProperty> GetMaterialPropertyAndAdvanceIterator(XMLParserHelper::iterator_t& propertiesBeginIt, const XMLParserHelper::iterator_t& propertiesEndIt, std::string& outPropertyName)
   {
      auto propertyStartNode = XMLParserHelper::GetItByNodeName(propertiesBeginIt, propertiesEndIt, PROPERTY_START_NODE_NAME);
      auto propertyEndNode = XMLParserHelper::GetItByNodeName(propertiesBeginIt, propertiesEndIt, PROPERTY_END_NODE_NAME);

      assert(propertyStartNode != propertiesEndIt);

      std::string propertyName, propertyType;

      ++propertyStartNode;
      for (auto it = propertyStartNode; it != propertyEndNode; ++it)
      {
         const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

         if (EngineUtility::StartsWith(currentNodeStr, "name"))
         {
            propertyName = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "name");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "type"))
         {
            propertyType = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "type");
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

      auto generalStartNode = XMLParserHelper::GetItByNodeName(fileSource, GENERAL_START_NODE_NAME);
      auto generalEndNode = XMLParserHelper::GetItByNodeName(fileSource, GENERAL_END_NODE_NAME);

      ++generalStartNode;
      for (auto it = generalStartNode; it != generalEndNode; ++it)
      {
         const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

         if (EngineUtility::StartsWith(currentNodeStr, "name"))
         {
            materialName = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "name");
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "shader"))
         {
            materialShaderName = XMLParserHelper::GetPropertyNodeByName(currentNodeStr, "shader");
         }
      }

      IMaterial* material = new IMaterial(materialName, materialShaderName);

      auto propertiesStartNode = XMLParserHelper::GetItByNodeName(fileSource, PROPERTIES_START_NODE_NAME);
      auto propertiesEndNode = XMLParserHelper::GetItByNodeName(fileSource, PROPERTIES_END_NODE_NAME);

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