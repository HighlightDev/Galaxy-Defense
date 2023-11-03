#pragma once
#include "Core/CommonCore/XMLParserHelper.h"
#include "Core/GraphicsCore/Material/DynamicMaterialOperations/MaterialNode.h"

#include <glm/vec2.hpp>
#include <memory>

using namespace Common;

namespace Graphics
{

#define UNARY_INCR_OP_START "<increment>"
#define UNARY_INCR_OP_END "</increment>"
#define UNARY_DECR_OP_START "<decrement>"
#define UNARY_DECR_OP_END "</decrement>"
#define UNARY_NO_OP_START "<no_op>"
#define UNARY_NO_OP_END "<no_op/>"
#define BINARY_ADD_OP_START "<add>"
#define BINARY_ADD_OP_END "</add>"
#define BINARY_SUB_OP_START "<sub>"
#define BINARY_SUB_OP_END "</sub>"
#define BINARY_MUL_OP_START "<mul>"
#define BINARY_MUL_OP_END "</mul>"
#define BINARY_DIV_OP_START "<div>"
#define BINARY_DIV_OP_END "</div>"

#define FLOAT_CONSTANT_START "<float_constant>"
#define FLOAT_CONSTANT_END "</float_constant>"

   struct MaterialNode;

   class MaterialNodeDecorator
   {
   public:
      MaterialNodeDecorator();
      ~MaterialNodeDecorator();

      glm::vec2 GetValueRange(const std::string &nodeStr) const;

      std::shared_ptr<MaterialNode> CreateMaterialOperationNode(std::string const &nodeName,
                                                                const MaterialNode::eMaterialPropertyType materialPropertyType) const;

      std::shared_ptr<MaterialNode> CreateValueNode(const std::string &propertyName,
                                                    const std::string &propertyValue) const;

      XMLParserHelper::iterator_t GetTagWithName(const std::string &operationName, XMLParserHelper::iterator_t &propertiesBeginIt,
                                                 const XMLParserHelper::iterator_t &propertiesEndIt);

      template <typename... NamesT>
      XMLParserHelper::iterator_t GetOneOfTagWithNames(XMLParserHelper::iterator_t &propertiesBeginIt,
                                                       const XMLParserHelper::iterator_t &propertiesEndIt,
                                                       NamesT &&...operationNames)
      {
         std::vector<std::string> operationTags = {std::forward<NamesT>(operationNames)...};
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
   };

}
