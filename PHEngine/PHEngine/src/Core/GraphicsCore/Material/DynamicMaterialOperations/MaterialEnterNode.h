#pragma once
#include "MaterialNode.h"

namespace Graphics {

struct MaterialEnterNode : public MaterialNode {
private:
    std::shared_ptr<MaterialNode> mInputOperation;

public:
    MaterialEnterNode();

    ~MaterialEnterNode() override;

    virtual eMaterialNodeType GetMaterialNodeType() const;

    eMaterialOperationType GetMaterialOperationType() const override;

    void AttachInputNode(std::shared_ptr<MaterialNode> inputNode) override;

    std::any GetValue();

    void SetPropertyType(const MaterialNode::eMaterialPropertyType propertyType);

protected:
    std::any TraverseGraph() override;
};
} // namespace Graphics
