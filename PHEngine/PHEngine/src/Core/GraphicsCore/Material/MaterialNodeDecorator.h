#pragma once
#include "Core/CommonCore/XMLParserHelper.h"

#include <glm/vec2.hpp>

using namespace Common;

namespace Graphics
{

#define UNARY_INCR_OP_START "<increment>"
#define UNARY_INCR_OP_END "</increment>"
#define BINARY_ADD_OP_START "<add>"
#define BINARY_ADD_OP_END "</add>"
#define BINARY_MUL_OP_START "<mul>"
#define BINARY_MUL_OP_END "</mul>"

#define FLOAT_CONSTANT_START "<float_constant>"
#define FLOAT_CONSTANT_END "</float_constant>"

   struct MaterialNode;

   class MaterialNodeDecorator
   {
   public:
      MaterialNodeDecorator();
      ~MaterialNodeDecorator();

      glm::vec2 GetValueRange(const std::string& nodeStr) const;

      std::shared_ptr<MaterialNode> CreateMaterialNode(std::string const& nodeName) const;

      std::shared_ptr<MaterialNode> CreateValueNode(const std::string& propertyName, const std::string& propertyValue) const;

      XMLParserHelper::iterator_t GetTagWithName(const std::string& operationName, XMLParserHelper::iterator_t& propertiesBeginIt,
         const XMLParserHelper::iterator_t& propertiesEndIt);

      template <typename... NamesT>
      XMLParserHelper::iterator_t GetOneOfTagWithNames(XMLParserHelper::iterator_t& propertiesBeginIt,
         const XMLParserHelper::iterator_t& propertiesEndIt, NamesT&&... operationNames)
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
   };

}
