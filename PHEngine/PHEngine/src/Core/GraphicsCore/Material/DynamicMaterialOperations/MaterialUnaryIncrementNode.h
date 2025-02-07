#pragma once
#include "MaterialUnaryOperationNode.h"

namespace Graphics {
struct MaterialUnaryIncrementNode : public MaterialUnaryOperationNode {
    explicit MaterialUnaryIncrementNode(const MaterialNode::eMaterialPropertyType materialPropertyType);

    virtual eMaterialOperationType GetMaterialOperationType() const;

    std::any DoOperation(const std::any& value) override;
};
} // namespace Graphics
