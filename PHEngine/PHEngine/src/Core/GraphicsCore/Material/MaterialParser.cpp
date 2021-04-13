#include "MaterialParser.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/IoCore/FileFacade.h"
#include "Core/CommonCore/XMLParserHelper.h"
#include "Core/GraphicsCore/Material/TextureMaterialProperty.h"
#include "Core/GraphicsCore/Material/FloatMaterialProperty.h"
#include "Core/GraphicsCore/Material/DeferredTextureMaterialProperty.h"
#include "Core/GraphicsCore/Material/DynamicFloatMaterialProperty.h"
#include "Core/GraphicsCore/Material/DynamicMaterialOperations/MaterialOperation.h"

using namespace Common;

namespace Graphics
{
#define GENERAL_START_NODE_NAME     "<general>"
#define GENERAL_END_NODE_NAME       "</general>"
#define PROPERTIES_START_NODE_NAME  "<properties>"
#define PROPERTIES_END_NODE_NAME    "</properties>"
#define PROPERTY_START_NODE_NAME    "<property>"
#define PROPERTY_END_NODE_NAME      "</property>"
#define DYNAMIC_PROPERTY_START_NODE_NAME  "<dynamic_property>"
#define DYNAMIC_PROPERTY_END_NODE_NAME  "</dynamic_property>"
#define DYNAMIC_PROPERTY_OPERATION_START_NODE_NAME "<operation>"
#define DYNAMIC_PROPERTY_OPERATION_END_NODE_NAME "</operation>"

#define UNARY_INCR_OP_START "<increment>"
#define UNARY_INCR_OP_END "</increment>"
#define BINARY_ADD_OP_START "<add>"
#define BINARY_ADD_OP_END "</add>"
#define BINARY_MUL_OP_START "<mul>"
#define BINARY_MUL_OP_END "</mul>"

#define FLOAT_VALUE_START "<float_value>"
#define FLOAT_VALUE_END "</float_value>"

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
      else if ("deferred_texture" == propertyType)
      {
         resultProperty = std::make_shared<DeferredTextureMaterialProperty>();
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
            propertyName = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "type"))
         {
            propertyType = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
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

      std::string materialName = "";
      std::string materialShaderName = "";
      std::string materialType = "";

      auto generalStartNode = XMLParserHelper::GetItByNodeName(fileSource, GENERAL_START_NODE_NAME);
      auto generalEndNode = XMLParserHelper::GetItByNodeName(fileSource, GENERAL_END_NODE_NAME);

      ++generalStartNode;
      for (auto it = generalStartNode; it != generalEndNode; ++it)
      {
         const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

         if (EngineUtility::StartsWith(currentNodeStr, "name"))
         {
            materialName = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "shader"))
         {
            materialShaderName = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
         }
         else if (EngineUtility::StartsWith(currentNodeStr, "material_type"))
         {
            materialType = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
         }
      }

      IMaterial* parsedMaterial = nullptr;

      if ("dynamic" == materialType)
      {
         parsedMaterial = ParseDynamicMaterial(fileSource, materialName, materialShaderName);
      }
      else if ("static" == materialType)
      {
         parsedMaterial = ParseStaticMaterial(fileSource, materialName, materialShaderName);
      }
      else {
         assert(false);
      }

      return parsedMaterial;
   }

   IMaterial* MaterialParser::ParseStaticMaterial(const std::list<std::string>& materialSrc, const std::string& materialName, const std::string& materialShaderPath)
   {
      IMaterial* material = new IMaterial(materialName, materialShaderPath);

      auto propertiesStartNode = XMLParserHelper::GetItByNodeName(materialSrc, PROPERTIES_START_NODE_NAME);
      auto propertiesEndNode = XMLParserHelper::GetItByNodeName(materialSrc, PROPERTIES_END_NODE_NAME);

      ++propertiesStartNode;
      for (auto it = propertiesStartNode; it != propertiesEndNode; ++it)
      {
         std::string propertyName;
         std::shared_ptr<MaterialProperty> materialProperty = GetMaterialPropertyAndAdvanceIterator(it, propertiesEndNode, propertyName);
         material->PushMaterialProperty(propertyName, std::move(materialProperty));
      }

      return material;
   }

   XMLParserHelper::iterator_t GetTagWithName(const std::string& operationName, XMLParserHelper::iterator_t& propertiesBeginIt, const XMLParserHelper::iterator_t& propertiesEndIt)
   {
      return XMLParserHelper::GetItByNodeName(propertiesBeginIt, propertiesEndIt, operationName);
   }
   template <typename... NamesT>
   XMLParserHelper::iterator_t GetOneOfTagWithNames(XMLParserHelper::iterator_t& propertiesBeginIt, const XMLParserHelper::iterator_t& propertiesEndIt, NamesT&&... operationNames)
   {
      std::vector<std::string> operationTags = { std::forward<NamesT>(operationNames)... };
      for (auto tag : operationTags)
      {
         auto it = GetTagWithName(tag, propertiesBeginIt, propertiesEndIt);
         if (it != propertiesEndIt)
         {
            return it;
         }
      }

      return propertiesEndIt;
   }

   XMLParserHelper::iterator_t ProcessDynamicProperty(const std::string& propertyType, std::shared_ptr<MaterialNode> node, XMLParserHelper::iterator_t& propertiesBeginIt, const XMLParserHelper::iterator_t& propertiesEndIt)
   {
      XMLParserHelper::iterator_t lastProcessedIt = propertiesBeginIt;

      while (lastProcessedIt != propertiesEndIt)
      {
         auto next = lastProcessedIt;
         next++;
         auto operationIt = GetOneOfTagWithNames(lastProcessedIt, next, UNARY_INCR_OP_START, BINARY_ADD_OP_START, BINARY_MUL_OP_START);
         auto valueIt = GetOneOfTagWithNames(lastProcessedIt, next, FLOAT_VALUE_START);
         const bool bOperation = operationIt != next;
         const bool bValue = valueIt != next;

         if (bOperation)
         {
            std::shared_ptr<MaterialNode> operationNode = nullptr;
            const std::string& currentNodeStr = EngineUtility::TrimStart(*operationIt);
            if (EngineUtility::StartsWith(currentNodeStr, UNARY_INCR_OP_START))
            {
               operationNode = std::make_shared<MaterialUnaryIncrementNode>();
            }
            else if (EngineUtility::StartsWith(currentNodeStr, BINARY_ADD_OP_START))
            {
               operationNode = std::make_shared < MaterialBinaryAddOperationNode>();
            }
            else if (EngineUtility::StartsWith(currentNodeStr, BINARY_MUL_OP_START))
            {
               operationNode = std::make_shared < MaterialBinaryMulOperationNode>();
            }

            switch (node->GetMaterialNodeType())
            {
               case MaterialNode::eMaterialNodeType::START:
               {
                  auto startNode = std::static_pointer_cast<MaterialStartNode>(node);
                  startNode->InputOperation = operationNode;
                  break;
               }
               case MaterialNode::eMaterialNodeType::VALUE:
               {
                  break;
               }
               case MaterialNode::eMaterialNodeType::UNARY_OP:
               {
                  auto unaryNode = std::static_pointer_cast<MaterialUnaryOperationNode>(node);
                  unaryNode->InputOperation = operationNode;
                  break;
               }
               case MaterialNode::eMaterialNodeType::BINARY_OP:
               {
                  auto binaryNode = std::static_pointer_cast<MaterialBinaryOperationNode>(node);
                  if (binaryNode->InputOperation1 == nullptr)
                  {
                     binaryNode->InputOperation1 = operationNode;
                  }
                  else if (binaryNode->InputOperation2 == nullptr)
                  {
                     binaryNode->InputOperation2 = operationNode;
                  }

                  break;
               }
            }

            lastProcessedIt = ++operationIt;
            lastProcessedIt = ProcessDynamicProperty(propertyType, operationNode, operationIt, propertiesEndIt);
         }
         else if (bValue)
         {
            const std::string& currentNodeStr = EngineUtility::TrimStart(*valueIt);
            lastProcessedIt = ++valueIt;
            float floatValue = 0.0f;
            if (EngineUtility::StartsWith(currentNodeStr, "<float_value>"))
            {
               const std::string& value = XMLParserHelper::GetPropertyNodeAfterColon(*lastProcessedIt);
               floatValue = std::stof(value);
            }

            switch (node->GetMaterialNodeType())
            {
               case MaterialNode::eMaterialNodeType::UNARY_OP:
               {
                  auto unaryNode = std::static_pointer_cast<MaterialUnaryOperationNode>(node);
                  auto valueNode = std::make_shared<MaterialValueNode>(floatValue);
                  unaryNode->InputOperation = valueNode;
                  break;
               }
               case MaterialNode::eMaterialNodeType::BINARY_OP:
               {
                  auto binaryNode = std::static_pointer_cast<MaterialBinaryOperationNode>(node);
                  auto valueNode = std::make_shared<MaterialValueNode>(floatValue);

                  if (binaryNode->InputOperation1 == nullptr)
                  {
                     binaryNode->InputOperation1 = valueNode;
                  }
                  else if (binaryNode->InputOperation2 == nullptr)
                  {
                     binaryNode->InputOperation2 = valueNode;
                  }

                  break;
               }
            }

            ++lastProcessedIt;
         }
         else
         {
            ++lastProcessedIt;
         }
      }

      return lastProcessedIt;
   }

   float getIteratedValue(std::shared_ptr<MaterialNode> node)
   {
      float result = 0.0f;
      if (node->GetMaterialNodeType() == MaterialNode::eMaterialNodeType::START)
      {
         auto startNode = std::static_pointer_cast<MaterialStartNode>(node);
         result = getIteratedValue(startNode->InputOperation);
      }
      else if (node->GetMaterialNodeType() == MaterialNode::eMaterialNodeType::UNARY_OP)
      {
         auto unaryNode = std::static_pointer_cast<MaterialUnaryOperationNode>(node);
         result = getIteratedValue(unaryNode->InputOperation);
         result = unaryNode->doOperation(result);
      }
      else if (node->GetMaterialNodeType() == MaterialNode::eMaterialNodeType::BINARY_OP)
      {
         auto binaryNode = std::static_pointer_cast<MaterialBinaryOperationNode>(node);
         result = getIteratedValue(binaryNode->InputOperation1);
         result = binaryNode->doOperation(result, getIteratedValue(binaryNode->InputOperation2));
      }
      else if (node->GetMaterialNodeType() == MaterialNode::eMaterialNodeType::VALUE)
      {
         auto valueNode = std::static_pointer_cast<MaterialValueNode>(node);
         result = valueNode->Value;
      }
      else {
         assert(false);
      }

      return result;
   }

   std::shared_ptr<MaterialProperty> GetMaterialDynamicPropertyAndAdvanceIterator(XMLParserHelper::iterator_t& propertiesBeginIt, const XMLParserHelper::iterator_t& propertiesEndIt, std::string& outPropertyName)
   {
      auto dynamicPropertyStartNode = XMLParserHelper::GetItByNodeName(propertiesBeginIt, propertiesEndIt, DYNAMIC_PROPERTY_START_NODE_NAME);
      auto dynamicPropertyEndNode = XMLParserHelper::GetItByNodeName(propertiesBeginIt, propertiesEndIt, DYNAMIC_PROPERTY_END_NODE_NAME);

      std::string propertyName = "", propertyType = "";

      ++dynamicPropertyStartNode;

      std::shared_ptr<MaterialStartNode> operation = std::make_shared<MaterialStartNode>();
      while (dynamicPropertyStartNode != dynamicPropertyEndNode)
      {
         const std::string& currentNodeStr = EngineUtility::TrimStart(*dynamicPropertyStartNode);

         if (EngineUtility::StartsWith(currentNodeStr, DYNAMIC_PROPERTY_OPERATION_START_NODE_NAME))
         {
            assert(propertyName != "" && propertyType != "");

            ProcessDynamicProperty(propertyType, operation, ++dynamicPropertyStartNode, dynamicPropertyEndNode);
            dynamicPropertyStartNode = dynamicPropertyEndNode;
         }
         else
         {
            if (EngineUtility::StartsWith(currentNodeStr, "name"))
            {
               propertyName = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
            }
            else if (EngineUtility::StartsWith(currentNodeStr, "type"))
            {
               propertyType = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
            }
            ++dynamicPropertyStartNode;
         }
      }

      outPropertyName = propertyName;
      propertiesBeginIt = dynamicPropertyEndNode;

      return std::make_shared<DynamicFloatMaterialProperty>(operation);
   }


   IMaterial* MaterialParser::ParseDynamicMaterial(const std::list<std::string>& materialSrc, const std::string& materialName, const std::string& materialShaderPath)
   {
      IMaterial* material = new IMaterial(materialName, materialShaderPath);

      auto propertiesStartNode = XMLParserHelper::GetItByNodeName(materialSrc, PROPERTIES_START_NODE_NAME);
      auto propertiesEndNode = XMLParserHelper::GetItByNodeName(materialSrc, PROPERTIES_END_NODE_NAME);

      ++propertiesStartNode;
      for (auto it = propertiesStartNode; it != propertiesEndNode; ++it)
      {
         const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

         std::string propertyName = "";
         std::shared_ptr<MaterialProperty> materialProperty;
         if (EngineUtility::StartsWith(currentNodeStr, DYNAMIC_PROPERTY_START_NODE_NAME))
         {
            materialProperty = GetMaterialDynamicPropertyAndAdvanceIterator(it, propertiesEndNode, propertyName);
            material->PushMaterialProperty(propertyName, std::move(materialProperty));
         }
         else
         {
            materialProperty = GetMaterialPropertyAndAdvanceIterator(it, propertiesEndNode, propertyName);
            material->PushMaterialProperty(propertyName, std::move(materialProperty));
         }
      }

      return material;
   }

#undef GENERAL_START_NODE_NAME    
#undef GENERAL_END_NODE_NAME      
#undef PROPERTIES_START_NODE_NAME 
#undef PROPERTIES_END_NODE_NAME   
#undef PROPERTY_START_NODE_NAME   
#undef PROPERTY_END_NODE_NAME    
#undef UNARY_INCR_OP_START
#undef UNARY_INCR_OP_END
#undef BINARY_ADD_OP_START
#undef BINARY_ADD_OP_END
#undef BINARY_MUL_OP_START
#undef BINARY_MUL_OP_END
#undef FLOAT_VALUE_START
#undef FLOAT_VALUE_END

}