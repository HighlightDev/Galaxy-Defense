#include "MaterialUnaryIncrementNode.h"

#include <glm/vec2.hpp>

namespace Graphics {
MaterialUnaryIncrementNode::MaterialUnaryIncrementNode(const MaterialNode::eMaterialPropertyType materialPropertyType)
    : MaterialUnaryOperationNode(materialPropertyType)
{
}

MaterialNode::eMaterialOperationType MaterialUnaryIncrementNode::GetMaterialOperationType() const
{
    return MaterialNode::eMaterialOperationType::UNARY_INCREMENT;
}

std::any MaterialUnaryIncrementNode::DoOperation(const std::any& value)
{
    if (MaterialNode::eMaterialPropertyType::FLOAT == GetMaterialPropertyType()) {
        return std::any_cast<float>(value) + 1.0f;
    } else if (MaterialNode::eMaterialPropertyType::IVEC2 == GetMaterialPropertyType()) {
        const auto vecVal = std::any_cast<glm::ivec2>(value);
        return vecVal + static_cast<decltype(vecVal.x)>(1);
    } else if (MaterialNode::eMaterialPropertyType::VEC2 == GetMaterialPropertyType()) {
        return std::any_cast<glm::vec2>(value) + 1.0f;
    }

    assert(false);
    return {};
}
} // namespace Graphics
