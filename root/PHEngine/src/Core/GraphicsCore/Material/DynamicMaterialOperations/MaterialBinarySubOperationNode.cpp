#include "MaterialBinarySubOperationNode.h"

#include <glm/vec2.hpp>

namespace Graphics {

MaterialBinarySubOperationNode::MaterialBinarySubOperationNode(const MaterialNode::eMaterialPropertyType materialPropertyType)
    : MaterialBinaryOperationNode(materialPropertyType)
{
}

MaterialNode::eMaterialOperationType MaterialBinarySubOperationNode::GetMaterialOperationType() const
{
    return MaterialNode::eMaterialOperationType::BINARY_SUB;
}

std::any MaterialBinarySubOperationNode::DoOperation(const std::any& left, const std::any& right)
{
    if (MaterialNode::eMaterialPropertyType::FLOAT == GetMaterialPropertyType()) {
        return std::any_cast<float>(left) - std::any_cast<float>(right);
    } else if (MaterialNode::eMaterialPropertyType::IVEC2 == GetMaterialPropertyType()) {
        return std::any_cast<glm::ivec2>(left) - std::any_cast<glm::ivec2>(right);
    } else if (MaterialNode::eMaterialPropertyType::VEC2 == GetMaterialPropertyType()) {
        return std::any_cast<glm::vec2>(left) - std::any_cast<glm::vec2>(right);
    }

    ext_assert(
        false,
        "MaterialBinarySubOperationNode::DoOperation: Unsupported material property type for BINARY_SUB operation: "
            + std::to_string(static_cast<int>(GetMaterialPropertyType())));
    return {};
}
} // namespace Graphics
