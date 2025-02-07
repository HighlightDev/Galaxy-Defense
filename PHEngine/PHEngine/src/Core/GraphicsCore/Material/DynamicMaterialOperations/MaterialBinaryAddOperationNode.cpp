#include "MaterialBinaryAddOperationNode.h"

#include <glm/vec2.hpp>

namespace Graphics {
MaterialBinaryAddOperationNode::MaterialBinaryAddOperationNode(const MaterialNode::eMaterialPropertyType materialPropertyType)
    : MaterialBinaryOperationNode(materialPropertyType)
{
}

MaterialNode::eMaterialOperationType MaterialBinaryAddOperationNode::GetMaterialOperationType() const
{
    return MaterialNode::eMaterialOperationType::BINARY_ADD;
}

std::any MaterialBinaryAddOperationNode::DoOperation(const std::any& left, const std::any& right)
{
    if (MaterialNode::eMaterialPropertyType::FLOAT == GetMaterialPropertyType()) {
        return std::any_cast<float>(left) + std::any_cast<float>(right);
    } else if (MaterialNode::eMaterialPropertyType::IVEC2 == GetMaterialPropertyType()) {
        return std::any_cast<glm::ivec2>(left) + std::any_cast<glm::ivec2>(right);
    } else if (MaterialNode::eMaterialPropertyType::VEC2 == GetMaterialPropertyType()) {
        return std::any_cast<glm::vec2>(left) + std::any_cast<glm::vec2>(right);
    }

    assert(false);
    return {};
}
} // namespace Graphics
