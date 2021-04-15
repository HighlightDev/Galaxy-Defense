#include "MaterialParser.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/IoCore/FileFacade.h"
#include "Core/CommonCore/XMLParserHelper.h"
#include "Core/GraphicsCore/Material/DynamicMaterial.h"
#include "Core/GraphicsCore/Material/MaterialProperties/TextureMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/FloatMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/DeferredTextureMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/DynamicFloatMaterialProperty.h"
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

#define FLOAT_CONSTANT_START "<float_constant>"
#define FLOAT_CONSTANT_END "</float_constant>"
#define PROPERTY_BINDING_START "<binding_property>"
#define PROPERTY_BINDING_END "</binding_property>"

   std::shared_ptr<MaterialProperty> CreatePropertyByType(const std::string& propertyType, const std::string& propertyName)
   {
      std::shared_ptr<MaterialProperty> resultProperty;

      if ("texture" == propertyType)
      {
         resultProperty = std::make_shared<TextureMaterialProperty>(propertyName);
      }
      else if ("float" == propertyType)
      {
         resultProperty = std::make_shared<FloatMaterialProperty>(propertyName);
      }
      else if ("deferred_texture" == propertyType)
      {
         resultProperty = std::make_shared<DeferredTextureMaterialProperty>(propertyName);
      }
      else
      {
         assert((false, "Unknown property type."));
      }

      return resultProperty;
   }

   std::shared_ptr<MaterialProperty> GetMaterialPropertyAndAdvanceIterator(XMLParserHelper::iterator_t& propertiesBeginIt, const XMLParserHelper::iterator_t& propertiesEndIt)
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

      propertiesBeginIt = propertyEndNode;

      return CreatePropertyByType(propertyType, propertyName);
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
         std::shared_ptr<MaterialProperty> materialProperty = GetMaterialPropertyAndAdvanceIterator(it, propertiesEndNode);
         material->PushMaterialProperty(materialProperty);
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

   XMLParserHelper::iterator_t ProcessDynamicProperty(const std::string& propertyType, std::shared_ptr<MaterialNode> node,
      XMLParserHelper::iterator_t& propertiesBeginIt, const XMLParserHelper::iterator_t& propertiesEndIt, std::vector<std::shared_ptr<MaterialProperty>>& innerDynamicMaterialProperties)
   {
      XMLParserHelper::iterator_t lastProcessedIt = propertiesBeginIt;

      while (lastProcessedIt != propertiesEndIt)
      {
         auto next = lastProcessedIt;
         next++;
         auto operationIt = GetOneOfTagWithNames(lastProcessedIt, next, UNARY_INCR_OP_START, BINARY_ADD_OP_START, BINARY_MUL_OP_START);
         auto valueIt = GetOneOfTagWithNames(lastProcessedIt, next, FLOAT_CONSTANT_START, PROPERTY_START_NODE_NAME);
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
               operationNode = std::make_shared<MaterialBinaryAddOperationNode>();
            }
            else if (EngineUtility::StartsWith(currentNodeStr, BINARY_MUL_OP_START))
            {
               operationNode = std::make_shared<MaterialBinaryMulOperationNode>();
            }

            // todo: create materia node attach adapter to hide inside it all attachment logic
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
            lastProcessedIt = ProcessDynamicProperty(propertyType, operationNode, operationIt, propertiesEndIt, innerDynamicMaterialProperties);
         }
         else if (bValue)
         {
            std::shared_ptr<MaterialNode> valueNode = nullptr;

            const std::string& currentNodeStr = EngineUtility::TrimStart(*valueIt);
            auto currentNodeIt = valueIt;
            lastProcessedIt = ++valueIt;
            if (EngineUtility::StartsWith(currentNodeStr, FLOAT_CONSTANT_START))
            {
               const std::string& value = XMLParserHelper::GetPropertyNodeAfterColon(*lastProcessedIt);
               float floatValue = std::stof(value);
               valueNode = std::make_shared<MaterialConstantFloatValueNode>(floatValue);
            }
            else if (EngineUtility::StartsWith(currentNodeStr, PROPERTY_START_NODE_NAME))
            {
               auto materialProperty = GetMaterialPropertyAndAdvanceIterator(currentNodeIt, propertiesEndIt);
               valueNode = std::make_shared<MaterialPropertyValueNode>(materialProperty);
               innerDynamicMaterialProperties.emplace_back(std::move(materialProperty));
            }

            switch (node->GetMaterialNodeType())
            {
               case MaterialNode::eMaterialNodeType::UNARY_OP:
               {
                  auto unaryNode = std::static_pointer_cast<MaterialUnaryOperationNode>(node);
                  unaryNode->InputOperation = valueNode;
                  break;
               }
               case MaterialNode::eMaterialNodeType::BINARY_OP:
               {
                  auto binaryNode = std::static_pointer_cast<MaterialBinaryOperationNode>(node);

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

   std::shared_ptr<DynamicFloatMaterialProperty> GetMaterialDynamicPropertyAndAdvanceIterator(XMLParserHelper::iterator_t& propertiesBeginIt,
      const XMLParserHelper::iterator_t& propertiesEndIt)
   {
      auto dynamicPropertyStartNode = XMLParserHelper::GetItByNodeName(propertiesBeginIt, propertiesEndIt, DYNAMIC_PROPERTY_START_NODE_NAME);
      auto dynamicPropertyEndNode = XMLParserHelper::GetItByNodeName(propertiesBeginIt, propertiesEndIt, DYNAMIC_PROPERTY_END_NODE_NAME);

      std::string propertyName = "", propertyType = "";

      ++dynamicPropertyStartNode;

      std::shared_ptr<MaterialStartNode> operation = std::make_shared<MaterialStartNode>();
      std::vector<std::shared_ptr<MaterialProperty>> innerDynamicMaterialProperties;
      while (dynamicPropertyStartNode != dynamicPropertyEndNode)
      {
         const std::string& currentNodeStr = EngineUtility::TrimStart(*dynamicPropertyStartNode);

         if (EngineUtility::StartsWith(currentNodeStr, DYNAMIC_PROPERTY_OPERATION_START_NODE_NAME))
         {
            assert(propertyName != "" && propertyType != "");
            dynamicPropertyStartNode = ProcessDynamicProperty(propertyType, operation, ++dynamicPropertyStartNode, dynamicPropertyEndNode, innerDynamicMaterialProperties);
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

      propertiesBeginIt = dynamicPropertyEndNode;

      auto dynamicMaterialPropery = std::make_shared<DynamicFloatMaterialProperty>(operation, propertyName);

      if (innerDynamicMaterialProperties.size())
         dynamicMaterialPropery->SetInternalDynamicMaterialProperties(std::move(innerDynamicMaterialProperties));

      return dynamicMaterialPropery;
   }


   IMaterial* MaterialParser::ParseDynamicMaterial(const std::list<std::string>& materialSrc, const std::string& materialName, const std::string& materialShaderPath)
   {
      DynamicMaterial* material = new DynamicMaterial(materialName, materialShaderPath);

      auto propertiesStartNode = XMLParserHelper::GetItByNodeName(materialSrc, PROPERTIES_START_NODE_NAME);
      auto propertiesEndNode = XMLParserHelper::GetItByNodeName(materialSrc, PROPERTIES_END_NODE_NAME);

      ++propertiesStartNode;
      for (auto it = propertiesStartNode; it != propertiesEndNode; ++it)
      {
         const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

         if (EngineUtility::StartsWith(currentNodeStr, DYNAMIC_PROPERTY_START_NODE_NAME))
         {
            auto property = GetMaterialDynamicPropertyAndAdvanceIterator(it, propertiesEndNode);
            material->PushDynamicProperty(property);
         }
         else if (EngineUtility::StartsWith(currentNodeStr, PROPERTY_START_NODE_NAME))
         {
            auto property = GetMaterialPropertyAndAdvanceIterator(it, propertiesEndNode);
            material->PushMaterialProperty(property);
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
#undef FLOAT_CONSTANT_START
#undef FLOAT_CONSTANT_END
#undef PROPERTY_BINDING_START
#undef PROPERTY_BINDING_END 
}