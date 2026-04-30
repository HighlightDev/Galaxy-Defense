#include "MaterialParser.h"

#include "Core/GraphicsCore/Material/DynamicMaterial.h"
#include "Core/GraphicsCore/Material/DynamicMaterialOperations/MaterialEnterNode.h"
#include "Core/GraphicsCore/Material/DynamicMaterialOperations/MaterialValuePropertyNode.h"
#include "Core/GraphicsCore/Material/MaterialProperties/DeferredTextureMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/DynamicMaterialProperties/DynamicFloatMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/DynamicMaterialProperties/DynamicIVec2MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/DynamicMaterialProperties/DynamicInstancedFloatMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/DynamicMaterialProperties/DynamicMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/DynamicMaterialProperties/DynamicVec2MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/DynamicMaterialProperties/DynamicVec3MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/FloatBindingMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/FloatMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/IntegerMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/TextureMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/Vec2BindingMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/Vec2MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/Vec3BindingMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/Vec3MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/iVec2BindingMaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/iVec2MaterialProperty.h"
#include "Core/IoCore/FileFacade.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"

#include <limits>
#include <unordered_map>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace Graphics {
#define GENERAL_START_NODE_NAME "<general>"
#define GENERAL_END_NODE_NAME "</general>"
#define PROPERTIES_START_NODE_NAME "<properties>"
#define PROPERTIES_END_NODE_NAME "</properties>"
#define PROPERTY_START_NODE_NAME "<property>"
#define PROPERTY_END_NODE_NAME "</property>"
#define DYNAMIC_PROPERTY_START_NODE_NAME "<dynamic_property>"
#define DYNAMIC_PROPERTY_END_NODE_NAME "</dynamic_property>"
#define INSTANCED_DYNAMIC_PROPERTY_START_NODE_NAME "<instanced_dynamic_property>"
#define INSTANCED_DYNAMIC_PROPERTY_END_NODE_NAME "</instanced_dynamic_property>"
#define DYNAMIC_PROPERTY_OPERATION_START_NODE_NAME "<operation>"
#define DYNAMIC_PROPERTY_OPERATION_END_NODE_NAME "</operation>"

std::shared_ptr<MaterialProperty> CreatePropertyByType(const std::string& propertyType, const std::string& propertyName)
{
    std::shared_ptr<MaterialProperty> resultProperty;

    if ("texture" == propertyType) {
        resultProperty = std::make_shared<TextureMaterialProperty>(propertyName);
    } else if ("integer" == propertyType) {
        resultProperty = std::make_shared<IntegerMaterialProperty>(propertyName);
    } else if ("float" == propertyType) {
        resultProperty = std::make_shared<FloatMaterialProperty>(propertyName);
    } else if ("deferred_texture" == propertyType) {
        resultProperty = std::make_shared<DeferredTextureMaterialProperty>(propertyName);
    } else if ("ivec2" == propertyType) {
        resultProperty = std::make_shared<iVec2MaterialProperty>(propertyName);
    } else if ("vec2" == propertyType) {
        resultProperty = std::make_shared<Vec2MaterialProperty>(propertyName);
    } else if ("vec3" == propertyType) {
        resultProperty = std::make_shared<Vec3MaterialProperty>(propertyName);
    } else if ("binding_float" == propertyType) {
        resultProperty
            = std::make_shared<FloatBindingMaterialProperty>(std::make_shared<FloatPropertyBinding>(propertyName), propertyName);
    } else if ("binding_ivec2" == propertyType) {
        resultProperty
            = std::make_shared<iVec2BindingMaterialProperty>(std::make_shared<iVec2PropertyBinding>(propertyName), propertyName);
    } else if ("binding_vec2" == propertyType) {
        resultProperty
            = std::make_shared<Vec2BindingMaterialProperty>(std::make_shared<Vec2PropertyBinding>(propertyName), propertyName);
    } else if ("binding_vec3" == propertyType) {
        resultProperty
            = std::make_shared<Vec3BindingMaterialProperty>(std::make_shared<Vec3PropertyBinding>(propertyName), propertyName);
    } else {
        ext_assert(false, "Unknown property type.");
    }

    return resultProperty;
}

std::shared_ptr<MaterialProperty> GetMaterialPropertyAndAdvanceIterator(
    XMLParserHelper::iterator_t& propertiesBeginIt, const XMLParserHelper::iterator_t& propertiesEndIt)
{
    auto propertyStartNode = XMLParserHelper::GetItByNodeName(propertiesBeginIt, propertiesEndIt, PROPERTY_START_NODE_NAME);
    auto propertyEndNode = XMLParserHelper::GetItByNodeName(propertiesBeginIt, propertiesEndIt, PROPERTY_END_NODE_NAME);

    ext_assert(
        propertyStartNode != propertiesEndIt,
        "MaterialParser::GetMaterialPropertyAndAdvanceIterator: Property start node not found");

    std::string propertyName, propertyType;

    ++propertyStartNode;
    for (auto it = propertyStartNode; it != propertyEndNode; ++it) {
        const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

        if (EngineUtility::StartsWith(currentNodeStr, "name")) {
            propertyName = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else if (EngineUtility::StartsWith(currentNodeStr, "type")) {
            propertyType = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        }
    }

    propertiesBeginIt = propertyEndNode;

    return CreatePropertyByType(propertyType, propertyName);
}

std::string MaterialParser::ReadMaterialNameFromMaterialDescriptor(const std::string& materialFileName)
{
    const std::string& absolutePath = IO::FolderManager::GetInstance()->GetMaterialPath() + materialFileName;
    FileFacade fileWorker;
    fileWorker.OpenAndReadFile(absolutePath);

    const size_t sizeOfSrc = fileWorker.GetFileSourceLinesCount();
    ext_assert(sizeOfSrc > 0, "MaterialParser::ReadMaterialNameFromMaterialDescriptor: File source is empty");

    std::list<std::string> fileSource = fileWorker.GetFileSrc();

    std::string materialName = "";

    auto generalStartNode = XMLParserHelper::GetItByNodeName(fileSource, GENERAL_START_NODE_NAME);
    auto generalEndNode = XMLParserHelper::GetItByNodeName(fileSource, GENERAL_END_NODE_NAME);

    ++generalStartNode;
    for (auto it = generalStartNode; it != generalEndNode; ++it) {
        const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

        if (EngineUtility::StartsWith(currentNodeStr, "name")) {
            materialName = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        }
    }

    ext_assert(materialName != "", "MaterialParser::ReadMaterialNameFromMaterialDescriptor: Material name is empty");
    return materialName;
}

std::shared_ptr<IMaterial> MaterialParser::ParseMaterialDescriptor(const std::string& materialFileName)
{
    const std::string& absolutePath = IO::FolderManager::GetInstance()->GetMaterialPath() + materialFileName;
    FileFacade fileWorker;
    fileWorker.OpenAndReadFile(absolutePath);

    const size_t sizeOfSrc = fileWorker.GetFileSourceLinesCount();
    ext_assert(sizeOfSrc > 0, "MaterialParser::ParseMaterialDescriptor: File source is empty");

    std::list<std::string> fileSource = fileWorker.GetFileSrc();

    std::string materialName = "";
    std::string materialShaderName = "";
    std::string materialType = "";

    auto generalStartNode = XMLParserHelper::GetItByNodeName(fileSource, GENERAL_START_NODE_NAME);
    auto generalEndNode = XMLParserHelper::GetItByNodeName(fileSource, GENERAL_END_NODE_NAME);

    ++generalStartNode;
    for (auto it = generalStartNode; it != generalEndNode; ++it) {
        const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

        if (EngineUtility::StartsWith(currentNodeStr, "name")) {
            materialName = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else if (EngineUtility::StartsWith(currentNodeStr, "shader")) {
            materialShaderName = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else if (EngineUtility::StartsWith(currentNodeStr, "material_type")) {
            materialType = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        }
    }

    std::shared_ptr<IMaterial> parsedMaterial;

    if ("dynamic" == materialType) {
        parsedMaterial = ParseDynamicMaterial(fileSource, materialName, materialShaderName);
    } else if ("static" == materialType) {
        parsedMaterial = ParseStaticMaterial(fileSource, materialName, materialShaderName);
    } else {
        ext_assert(false, "MaterialParser::ParseMaterialDescriptor: Unknown material type: " + materialType);
    }

    return parsedMaterial;
}

std::shared_ptr<IMaterial> MaterialParser::ParseStaticMaterial(
    const std::list<std::string>& materialSrc, const std::string& materialName, const std::string& materialShaderPath)
{
    const auto materialInstance = std::make_shared<IMaterial>(materialName, materialShaderPath);

    auto propertiesStartNode = XMLParserHelper::GetItByNodeName(materialSrc, PROPERTIES_START_NODE_NAME);
    auto propertiesEndNode = XMLParserHelper::GetItByNodeName(materialSrc, PROPERTIES_END_NODE_NAME);

    ++propertiesStartNode;
    for (auto it = propertiesStartNode; it != propertiesEndNode; ++it) {
        std::shared_ptr<MaterialProperty> materialProperty = GetMaterialPropertyAndAdvanceIterator(it, propertiesEndNode);
        materialInstance->PushMaterialProperty(materialProperty);
    }

    return materialInstance;
}

XMLParserHelper::iterator_t MaterialParser::ProcessDynamicProperty(
    const std::string& propertyType,
    std::shared_ptr<MaterialNode> node,
    XMLParserHelper::iterator_t& propertiesBeginIt,
    const XMLParserHelper::iterator_t& propertiesEndIt,
    std::vector<std::shared_ptr<MaterialProperty>>& innerDynamicMaterialProperties)
{
    XMLParserHelper::iterator_t lastProcessedIt = propertiesBeginIt;

    while (lastProcessedIt != propertiesEndIt) {
        auto next = lastProcessedIt;
        next++;

        bool bOperation = false, bValue = false;
        auto nodeIt = mMaterialNodeDecorator.GetOneOfTagWithNames(
            lastProcessedIt,
            next,
            UNARY_NO_OP_START,
            UNARY_INCR_OP_START,
            UNARY_DECR_OP_START,
            BINARY_ADD_OP_START,
            BINARY_SUB_OP_START,
            BINARY_MUL_OP_START,
            BINARY_DIV_OP_START);
        bOperation = nodeIt != next;

        if (!bOperation) {
            nodeIt = mMaterialNodeDecorator.GetOneOfTagWithNames(
                lastProcessedIt, next, FLOAT_CONSTANT_START, PROPERTY_START_NODE_NAME);
            bValue = nodeIt != next;
        }

        if (bOperation) {
            const std::string& currentNodeStr = EngineUtility::TrimStart(*nodeIt);

            const MaterialNode::eMaterialPropertyType materialPropType
                = ConvertPropertyStrToPropertyTypeForMaterialOperationNode(propertyType);
            std::shared_ptr<MaterialNode> operationNode
                = mMaterialNodeDecorator.CreateMaterialOperationNode(currentNodeStr, materialPropType);
            node->AttachInputNode(operationNode);

            lastProcessedIt = ++nodeIt;
            lastProcessedIt
                = ProcessDynamicProperty(propertyType, operationNode, nodeIt, propertiesEndIt, innerDynamicMaterialProperties);
        } else if (bValue) {
            const std::string& currentNodeStr = EngineUtility::TrimStart(*nodeIt);
            auto currentNodeIt = nodeIt;
            lastProcessedIt = ++nodeIt;

            std::shared_ptr<MaterialNode> valueNode = nullptr;

            if (EngineUtility::StartsWith(currentNodeStr, PROPERTY_START_NODE_NAME)) {
                auto materialProperty = GetMaterialPropertyAndAdvanceIterator(currentNodeIt, propertiesEndIt);
                const std::unordered_map<MaterialProperty::eMaterialPropertyType, MaterialNode::eMaterialPropertyType>
                    supportedDynamicProperties
                    = {{MaterialProperty::eMaterialPropertyType::FLOAT_PROPERTY, MaterialNode::eMaterialPropertyType::FLOAT},
                       {MaterialProperty::eMaterialPropertyType::FLOAT_BINDING_PROPERTY,
                        MaterialNode::eMaterialPropertyType::FLOAT},
                       {MaterialProperty::eMaterialPropertyType::IVEC2_PROPERTY, MaterialNode::eMaterialPropertyType::IVEC2},
                       {MaterialProperty::eMaterialPropertyType::IVEC2_BINDING_PROPERTY,
                        MaterialNode::eMaterialPropertyType::IVEC2},
                       {MaterialProperty::eMaterialPropertyType::VEC2_PROPERTY, MaterialNode::eMaterialPropertyType::VEC2},
                       {MaterialProperty::eMaterialPropertyType::VEC2_BINDING_PROPERTY,
                        MaterialNode::eMaterialPropertyType::VEC2},
                       {MaterialProperty::eMaterialPropertyType::VEC3_BINDING_PROPERTY,
                        MaterialNode::eMaterialPropertyType::VEC3}};

                ext_assert(
                    supportedDynamicProperties.count(materialProperty->GetPropertyType()),
                    "MaterialParser::ParseMaterialDescriptor: Unsupported dynamic property type: "
                        + materialProperty->GetPropertyName());
                valueNode = std::make_shared<MaterialValuePropertyNode>(
                    materialProperty, supportedDynamicProperties.at(materialProperty->GetPropertyType()));
                innerDynamicMaterialProperties.emplace_back(std::move(materialProperty));
            } else {
                valueNode = mMaterialNodeDecorator.CreateValueNode(currentNodeStr, *lastProcessedIt);
            }

            ext_assert(valueNode, "MaterialParser::ParseMaterialDescriptor: Value node creation failed");
            node->AttachInputNode(valueNode);
            ++lastProcessedIt;
        } else {
            ++lastProcessedIt;
        }
    }

    return lastProcessedIt;
}

std::shared_ptr<DynamicMaterialProperty> MaterialParser::GetMaterialInstancedDynamicPropertyAndAdvanceIterator(
    XMLParserHelper::iterator_t& propertiesBeginIt, const XMLParserHelper::iterator_t& propertiesEndIt)
{
    auto instancedDynamicPropertyStartNode
        = XMLParserHelper::GetItByNodeName(propertiesBeginIt, propertiesEndIt, INSTANCED_DYNAMIC_PROPERTY_START_NODE_NAME);
    auto instancedDynamicPropertyEndNode
        = XMLParserHelper::GetItByNodeName(propertiesBeginIt, propertiesEndIt, INSTANCED_DYNAMIC_PROPERTY_END_NODE_NAME);

    std::string propertyName = "", propertyType = "";
    bool propertyValueIncremental = false;

    glm::vec2 minMaxRange(std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
    bool minMaxRangeExists = false;

    ++instancedDynamicPropertyStartNode;

    while (instancedDynamicPropertyStartNode != instancedDynamicPropertyEndNode) {
        const std::string& currentNodeStr = EngineUtility::TrimStart(*instancedDynamicPropertyStartNode);
        if (EngineUtility::StartsWith(currentNodeStr, "name")) {
            propertyName = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else if (EngineUtility::StartsWith(currentNodeStr, "type")) {
            propertyType = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else if (EngineUtility::StartsWith(currentNodeStr, "incremental")) {
            propertyValueIncremental = "true" == XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
        } else if (EngineUtility::StartsWith(currentNodeStr, "range")) {
            minMaxRange = mMaterialNodeDecorator.GetValueRange(currentNodeStr);
            minMaxRangeExists = true;
        }
        ++instancedDynamicPropertyStartNode;
    }

    propertiesBeginIt = instancedDynamicPropertyEndNode;

    ext_assert(
        propertyName != "" && propertyType != "",
        "MaterialParser::GetMaterialInstancedDynamicPropertyAndAdvanceIterator: Property name or type is empty");

    std::shared_ptr<DynamicMaterialProperty> dynamicMaterialPropery;
    if ("instanced_binding_float" == propertyType) {
        auto instancedFloatProperty = std::make_shared<DynamicInstancedFloatMaterialProperty>(propertyName);
        instancedFloatProperty->SetIsValueIncremental(propertyValueIncremental);
        if (minMaxRangeExists) {
            instancedFloatProperty->SetRange(minMaxRange);
        }
        dynamicMaterialPropery = instancedFloatProperty;
    } else {
        ext_assert(
            false,
            "MaterialParser::GetMaterialInstancedDynamicPropertyAndAdvanceIterator: Unknown instanced dynamic property type: "
                + propertyType);
    }

    ext_assert(
        dynamicMaterialPropery,
        "MaterialParser::GetMaterialInstancedDynamicPropertyAndAdvanceIterator: Dynamic material property creation failed");

    return dynamicMaterialPropery;
}

std::shared_ptr<DynamicMaterialProperty> MaterialParser::GetMaterialDynamicPropertyAndAdvanceIterator(
    XMLParserHelper::iterator_t& propertiesBeginIt, const XMLParserHelper::iterator_t& propertiesEndIt)
{
    auto dynamicPropertyStartNode
        = XMLParserHelper::GetItByNodeName(propertiesBeginIt, propertiesEndIt, DYNAMIC_PROPERTY_START_NODE_NAME);
    auto dynamicPropertyEndNode
        = XMLParserHelper::GetItByNodeName(propertiesBeginIt, propertiesEndIt, DYNAMIC_PROPERTY_END_NODE_NAME);

    std::string propertyName = "", propertyType = "";
    bool propertyValueIncremental = false;

    glm::vec2 minMaxRange(std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
    bool minMaxRangeExists = false;

    ++dynamicPropertyStartNode;

    std::shared_ptr<MaterialEnterNode> operationEnterNode = std::make_shared<MaterialEnterNode>();
    std::vector<std::shared_ptr<MaterialProperty>> innerDynamicMaterialProperties;
    while (dynamicPropertyStartNode != dynamicPropertyEndNode) {
        const std::string& currentNodeStr = EngineUtility::TrimStart(*dynamicPropertyStartNode);

        if (EngineUtility::StartsWith(currentNodeStr, DYNAMIC_PROPERTY_OPERATION_START_NODE_NAME)) {
            dynamicPropertyStartNode = ProcessDynamicProperty(
                propertyType,
                operationEnterNode,
                ++dynamicPropertyStartNode,
                dynamicPropertyEndNode,
                innerDynamicMaterialProperties);
        } else {
            if (EngineUtility::StartsWith(currentNodeStr, "name")) {
                propertyName = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
            } else if (EngineUtility::StartsWith(currentNodeStr, "type")) {
                propertyType = XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
            } else if (EngineUtility::StartsWith(currentNodeStr, "incremental")) {
                propertyValueIncremental = "true" == XMLParserHelper::GetPropertyNodeAfterColon(currentNodeStr);
            } else if (EngineUtility::StartsWith(currentNodeStr, "range")) {
                minMaxRange = mMaterialNodeDecorator.GetValueRange(currentNodeStr);
                minMaxRangeExists = true;
            }
            ++dynamicPropertyStartNode;
        }
    }

    propertiesBeginIt = dynamicPropertyEndNode;

    ext_assert(
        propertyName != "" && propertyType != "",
        "MaterialParser::GetMaterialDynamicPropertyAndAdvanceIterator: Property name or type is empty");
    operationEnterNode->SetPropertyType(ConvertPropertyStrToPropertyTypeForMaterialOperationNode(propertyType));

    std::shared_ptr<DynamicMaterialProperty> dynamicMaterialPropery;
    if ("float" == propertyType) {
        const auto dynamicFloatProperty = std::make_shared<DynamicFloatMaterialProperty>(operationEnterNode, propertyName);
        dynamicFloatProperty->SetIsValueIncremental(propertyValueIncremental);
        if (minMaxRangeExists) {
            dynamicFloatProperty->SetRange(minMaxRange);
        }
        dynamicMaterialPropery = dynamicFloatProperty;
    } else if ("vec2" == propertyType) {
        const auto dynamicVec2Property = std::make_shared<DynamicVec2MaterialProperty>(operationEnterNode, propertyName);
        dynamicVec2Property->SetIsValueIncremental(propertyValueIncremental);
        if (minMaxRangeExists) {
            dynamicVec2Property->SetRange(minMaxRange);
        }
        dynamicMaterialPropery = dynamicVec2Property;
    } else if ("ivec2" == propertyType) {
        const auto dynamicIVec2Property = std::make_shared<DynamicIVec2MaterialProperty>(operationEnterNode, propertyName);
        dynamicIVec2Property->SetIsValueIncremental(propertyValueIncremental);
        if (minMaxRangeExists) {
            dynamicIVec2Property->SetRange(glm::ivec2(static_cast<int32_t>(minMaxRange.x), static_cast<int32_t>(minMaxRange.y)));
        }
        dynamicMaterialPropery = dynamicIVec2Property;
    } else if ("vec3" == propertyType) {
        const auto dynamicVec3Property = std::make_shared<DynamicVec3MaterialProperty>(operationEnterNode, propertyName);
        dynamicVec3Property->SetIsValueIncremental(propertyValueIncremental);
        if (minMaxRangeExists) {
            dynamicVec3Property->SetRange(minMaxRange);
        }
        dynamicMaterialPropery = dynamicVec3Property;
    } else {
        ext_assert(
            false,
            "MaterialParser::GetMaterialDynamicPropertyAndAdvanceIterator: Unknown dynamic property type: " + propertyType);
    }

    ext_assert(
        dynamicMaterialPropery,
        "MaterialParser::GetMaterialDynamicPropertyAndAdvanceIterator: Dynamic material property creation failed");

    if (innerDynamicMaterialProperties.size()) {
        dynamicMaterialPropery->SetInternalDynamicMaterialProperties(std::move(innerDynamicMaterialProperties));
    }

    return dynamicMaterialPropery;
}

std::shared_ptr<IMaterial> MaterialParser::ParseDynamicMaterial(
    const std::list<std::string>& materialSrc, const std::string& materialName, const std::string& materialShaderPath)
{
    const auto materialInstance = std::make_shared<DynamicMaterial>(materialName, materialShaderPath);

    auto propertiesStartNode = XMLParserHelper::GetItByNodeName(materialSrc, PROPERTIES_START_NODE_NAME);
    auto propertiesEndNode = XMLParserHelper::GetItByNodeName(materialSrc, PROPERTIES_END_NODE_NAME);

    ++propertiesStartNode;
    for (auto it = propertiesStartNode; it != propertiesEndNode; ++it) {
        const std::string& currentNodeStr = EngineUtility::TrimStart(*it);

        if (EngineUtility::StartsWith(currentNodeStr, INSTANCED_DYNAMIC_PROPERTY_START_NODE_NAME)) {
            auto property = GetMaterialInstancedDynamicPropertyAndAdvanceIterator(it, propertiesEndNode);
            materialInstance->PushDynamicProperty(property);
        } else if (EngineUtility::StartsWith(currentNodeStr, DYNAMIC_PROPERTY_START_NODE_NAME)) {
            auto property = GetMaterialDynamicPropertyAndAdvanceIterator(it, propertiesEndNode);
            materialInstance->PushDynamicProperty(property);
        } else if (EngineUtility::StartsWith(currentNodeStr, PROPERTY_START_NODE_NAME)) {
            auto property = GetMaterialPropertyAndAdvanceIterator(it, propertiesEndNode);
            materialInstance->PushMaterialProperty(property);
        }
    }

    return materialInstance;
}

MaterialNode::eMaterialPropertyType
MaterialParser::ConvertPropertyStrToPropertyTypeForMaterialOperationNode(const std::string& propertyTypeStr) const
{
    const std::unordered_map<std::string, MaterialNode::eMaterialPropertyType> strToPropTypeMapping
        = {{"float", MaterialNode::eMaterialPropertyType::FLOAT},
           {"binding_float", MaterialNode::eMaterialPropertyType::FLOAT},
           {"ivec2", MaterialNode::eMaterialPropertyType::IVEC2},
           {"binding_ivec2", MaterialNode::eMaterialPropertyType::IVEC2},
           {"vec2", MaterialNode::eMaterialPropertyType::VEC2},
           {"binding_vec2", MaterialNode::eMaterialPropertyType::VEC2},
           {"vec3", MaterialNode::eMaterialPropertyType::VEC3},
           {"binding_vec3", MaterialNode::eMaterialPropertyType::VEC3}};

    ext_assert(
        strToPropTypeMapping.count(propertyTypeStr),
        "MaterialParser::ConvertPropertyStrToPropertyTypeForMaterialOperationNode: Unsupported property type string: "
            + propertyTypeStr);
    return strToPropTypeMapping.at(propertyTypeStr);
}

#undef GENERAL_START_NODE_NAME
#undef GENERAL_END_NODE_NAME
#undef PROPERTIES_START_NODE_NAME
#undef PROPERTIES_END_NODE_NAME
#undef PROPERTY_START_NODE_NAME
#undef PROPERTY_END_NODE_NAME
#undef INSTANCED_DYNAMIC_PROPERTY_START_NODE_NAME
#undef INSTANCED_DYNAMIC_PROPERTY_END_NODE_NAME
#undef DYNAMIC_PROPERTY_START_NODE_NAME
#undef DYNAMIC_PROPERTY_END_NODE_NAME
} // namespace Graphics