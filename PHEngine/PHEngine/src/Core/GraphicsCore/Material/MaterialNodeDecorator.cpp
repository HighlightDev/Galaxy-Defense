#include "MaterialNodeDecorator.h"
#include "Core/GraphicsCore/Material/DynamicMaterialOperations/MaterialOperation.h"

namespace Graphics
{
   MaterialNodeDecorator::MaterialNodeDecorator()
   {
   }

   MaterialNodeDecorator::~MaterialNodeDecorator()
   {
   }

   glm::vec2 MaterialNodeDecorator::GetValueRange(const std::string& nodeStr) const {

      glm::vec2 minMaxRange;

      auto rangeStr = XMLParserHelper::GetPropertyNodeAfterColon(nodeStr);
      const std::string& range = XMLParserHelper::GetSubstringInsideBrackets(rangeStr);
      auto rangeValues = EngineUtility::Split(range, ';');
      assert(rangeValues.size() == 2);
      const std::string& rangeMinValue = EngineUtility::TrimEnd(EngineUtility::TrimStart(rangeValues[0]));
      const std::string& rangeMaxValue = EngineUtility::TrimEnd(EngineUtility::TrimStart(rangeValues[1]));
      minMaxRange.x = std::stof(rangeMinValue);
      minMaxRange.y = std::stof(rangeMaxValue);

      return minMaxRange;
   }

   std::shared_ptr<MaterialNode> MaterialNodeDecorator::CreateValueNode(const std::string& propertyName, const std::string& propertyValue) const {
      std::shared_ptr<MaterialNode> valueNode = nullptr;
      
      if (EngineUtility::StartsWith(propertyName, FLOAT_CONSTANT_START))
      {
         const std::string& value = XMLParserHelper::GetPropertyNodeAfterColon(propertyValue);
         float floatValue = std::stof(value);
         valueNode = std::make_shared<MaterialConstantFloatValueNode>(floatValue);
      }

      return valueNode;
   }

   std::shared_ptr<MaterialNode> MaterialNodeDecorator::CreateMaterialNode(std::string const& nodeName) const
   {
      std::shared_ptr<MaterialNode> operationNode = nullptr;
      if (EngineUtility::StartsWith(nodeName, UNARY_INCR_OP_START))
      {
         operationNode = std::make_shared<MaterialUnaryIncrementNode>();
      }
      else if (EngineUtility::StartsWith(nodeName, BINARY_ADD_OP_START))
      {
         operationNode = std::make_shared<MaterialBinaryAddOperationNode>();
      }
      else if (EngineUtility::StartsWith(nodeName, BINARY_MUL_OP_START))
      {
         operationNode = std::make_shared<MaterialBinaryMulOperationNode>();
      }
      return operationNode;
   }

   XMLParserHelper::iterator_t MaterialNodeDecorator::GetTagWithName(const std::string& operationName, XMLParserHelper::iterator_t& propertiesBeginIt, const XMLParserHelper::iterator_t& propertiesEndIt)
   {
      return XMLParserHelper::GetItByNodeName(propertiesBeginIt, propertiesEndIt, operationName);
   }

}