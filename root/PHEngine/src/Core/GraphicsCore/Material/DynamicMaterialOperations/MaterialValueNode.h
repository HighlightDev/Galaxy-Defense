#pragma once
#include "MaterialNode.h"

namespace Graphics {
struct MaterialValueNode : public MaterialNode {
public:
    MaterialValueNode(const MaterialNode::eMaterialPropertyType materialPropertyType);

    ~MaterialValueNode() override;

    enum class eValueType { FLOAT_CONSTANT, PROPERTY };

    eMaterialNodeType GetMaterialNodeType() const override;

    virtual eMaterialOperationType GetMaterialOperationType() const;

    void AttachInputNode(std::shared_ptr<MaterialNode> inputNode) override;

    virtual eValueType GetValueType() const = 0;
};
} // namespace Graphics
