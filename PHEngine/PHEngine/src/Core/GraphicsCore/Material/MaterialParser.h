#pragma once

#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/MaterialNodeDecorator.h"

#include <list>
#include <memory>

namespace Graphics
{
   struct MaterialProperty;
   struct DynamicMaterialProperty;

   class MaterialParser
   {

      MaterialNodeDecorator mMaterialNodeDecorator;

   public:
      std::shared_ptr<IMaterial> ParseMaterialDescriptor(const std::string &materialFileName);

   private:
      std::shared_ptr<IMaterial> ParseStaticMaterial(const std::list<std::string> &materialSrc, const std::string &materialName, const std::string &materialShaderPath);

      std::shared_ptr<IMaterial> ParseDynamicMaterial(const std::list<std::string> &materialSrc, const std::string &materialName, const std::string &materialShaderPath);

      XMLParserHelper::iterator_t ProcessDynamicProperty(const std::string &propertyType, std::shared_ptr<MaterialNode> node,
                                                         XMLParserHelper::iterator_t &propertiesBeginIt, const XMLParserHelper::iterator_t &propertiesEndIt, std::vector<std::shared_ptr<MaterialProperty>> &innerDynamicMaterialProperties);

      std::shared_ptr<DynamicMaterialProperty> GetMaterialDynamicPropertyAndAdvanceIterator(XMLParserHelper::iterator_t &propertiesBeginIt,
                                                                                            const XMLParserHelper::iterator_t &propertiesEndIt);

      MaterialNode::eMaterialPropertyType ConvertPropertyStrToPropertyTypeForMaterialOperationNode(const std::string &propertyTypeStr) const;
   };
}
