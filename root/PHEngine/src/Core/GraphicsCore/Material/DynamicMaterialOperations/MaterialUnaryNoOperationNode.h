#pragma once
#include "MaterialUnaryOperationNode.h"

namespace Graphics {
struct MaterialUnaryNoOperationNode : public MaterialUnaryOperationNode {
    explicit MaterialUnaryNoOperationNode(const MaterialNode::eMaterialPropertyType materialPropertyType);

    virtual eMaterialOperationType GetMaterialOperationType() const;

    std::any DoOperation(const std::any& value) override;
};
} // namespace Graphics