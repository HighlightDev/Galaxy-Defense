#pragma once
#include "MaterialBinaryOperationNode.h"

namespace Graphics {
struct MaterialBinaryMulOperationNode : public MaterialBinaryOperationNode {
    MaterialBinaryMulOperationNode(const MaterialNode::eMaterialPropertyType materialPropertyType);

    eMaterialOperationType GetMaterialOperationType() const override;

    std::any DoOperation(const std::any& left, const std::any& right) override;
};
} // namespace Graphics
